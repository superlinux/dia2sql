#include <pugixml.hpp>
#include <pugiconfig.hpp>
#include <iostream>
#include <fstream>
#include <gzstream.h>
#include <map>
#include <vector>
//#include<sstream>
using namespace std;
using namespace pugi;
struct connection {
  string connection_id_from_dia_object_id="";
  string handle_zero_connection_object_id="";
  string handle_one_connection_object_id="";
} single_connection;
vector<connection> connections_list;
int table_counter=0;
string sql_create_table_predicate="",table_field_name=" ",table_field_type=" ",table_field_comment=" ",table_field_is_primary_key=" ",table_field_is_unique=" ",table_field_is_nullable=" ",table_field_default_value=" ", primary_key_predicate="", table_field_definitions_list_predicate="",unique_predicate="",default_value_predicate="";
string table_name, all_sql_statements="";
string input_file_path,output_file_path,argv_2,dia_database_diagram_contents_as_xml="",table_dia_object_id="";
std::string my_utf8(const char * str) { return str; }
std::string my_utf8(const wchar_t * str) { return as_utf8(str); }
bool verbose=false;
igzstream dia_database_diagram_file;
std::map<std::string, string> map_connection_handle_zero; //map dia:object id vs where it's connected to, for handle=zero
std::map<std::string, string> map_connection_handle_one; //map dia:object id vs where it's connected to,  for handle=one
std::map<std::string, string> map_dia_object_id_to_table_name;
string connection_id_from_dia_object_id="";
int main(int argc, char* argv[]) {
  //cout <<argc<<endl<<argv[0]<<endl;
 switch (argc) { 
  case 1: 
    cout<<"Usage: dia2sql  INPUT_FILE.dia  [ OUTPUT.SQL] [ --verbose ]."<< endl;
    cout<<"This command generates the SQL code for table creation from an INPUT_FILE.dia dia diagram file and save them in file OUTPUT.SQL"<<endl;
    cout<<"You have to use the shapes of [Database] in the menu of shapes "<<endl;
    cout<<"If OUTPUT.SQL is not given, then the SQL code will be displayed on the terminal to stdout."<<endl;
  return 0;
  
  case 2:
    input_file_path=argv[1];
    break;
  case 3:
    input_file_path=argv[1];
      argv_2=argv[2];
    if (argv_2=="--verbose"){
         cout<<"argv 2: "<<argv[2]<<endl;

      verbose=true;
      } else {
         output_file_path=argv[2];
       
      }
  break;
  case 4:
    input_file_path=argv[1];
    output_file_path=argv[2];
    verbose=true;
    
  break;  
}

dia_database_diagram_file.open(input_file_path.c_str());
if (! dia_database_diagram_file) {
 cerr<<"dia diagram file"<<input_file_path<<"does not exist and not found."<<endl<<endl;
 return -1;
  
}
string line;
if(verbose) cout << "Printing the dia diagram "<< input_file_path<< " as XML.." <<endl<<endl;
while(getline(dia_database_diagram_file, line)) {
   if(verbose)  cout << line << endl;
    dia_database_diagram_contents_as_xml+=line;
}
dia_database_diagram_file.close();
if(verbose) cout << endl<<endl<<"DONE Printing the dia diagram "<< input_file_path<< " as XML.." <<endl<<endl<<"Printing dia2sql analysis."<<endl<<endl;

   pugi::xml_document doc;
   string table_attribute_name;
    pugi::xml_parse_result result = doc.load_string(dia_database_diagram_contents_as_xml.c_str());
//     if (!result) {
//       cerr<<"input file not found or does not exists or it's not a dia diagram file"<<endl;
//         return -1;
//     }
//  
      for (pugi::xml_node xml_node_all_diagram_objects = doc.first_element_by_path("/dia:diagram/dia:layer/dia:object"); xml_node_all_diagram_objects; xml_node_all_diagram_objects = xml_node_all_diagram_objects.next_sibling()){

     if(verbose)   std::cout <<endl<<"Diagram shape #"<<++table_counter<<endl<< " dia_object name: " << xml_node_all_diagram_objects.name() <<" type="<<xml_node_all_diagram_objects.attribute("type").value()<<endl;
     
     
     
     
        if (my_utf8(xml_node_all_diagram_objects.attribute("type").value()) == "Database - Table") {
          table_dia_object_id=my_utf8(xml_node_all_diagram_objects.attribute("id").value());
            for (xml_node xml_node_list_of_tables = xml_node_all_diagram_objects.first_element_by_path("dia:attribute"); xml_node_list_of_tables; xml_node_list_of_tables = xml_node_list_of_tables.next_sibling()){
               if ( my_utf8( xml_node_list_of_tables.attribute("name").value()) =="name") {
               
                   
                   table_name=my_utf8(xml_node_list_of_tables.child_value("dia:string"));
                   table_name=table_name.substr(1,table_name.length()-2);
                   map_dia_object_id_to_table_name[table_dia_object_id]=table_name;
                  sql_create_table_predicate="create table if not exists "+table_name+" (";
               if(verbose)   cout<<"table name="<<table_name<<endl;
                   
               }
               if ( my_utf8( xml_node_list_of_tables.attribute("name").value()) =="attributes") {
                for (xml_node xml_node_list_of_table_fields_in_on_single_table = xml_node_list_of_tables.first_element_by_path("dia:composite"); xml_node_list_of_table_fields_in_on_single_table; xml_node_list_of_table_fields_in_on_single_table = xml_node_list_of_table_fields_in_on_single_table.next_sibling()){
                 if(verbose) cout<< endl;
                  for (xml_node xml_node_single_table_field = xml_node_list_of_table_fields_in_on_single_table.first_element_by_path("dia:attribute"); xml_node_single_table_field; xml_node_single_table_field = xml_node_single_table_field.next_sibling()){
                   table_attribute_name=my_utf8(xml_node_single_table_field.attribute("name").value());
                    if (table_attribute_name=="name") {
                     table_field_name=my_utf8(xml_node_single_table_field.child("dia:string").child_value());
                     table_field_name  = table_field_name.substr(1, table_field_name.length()-2);
                    if (table_field_name.empty()) {
                        cerr<<"Error! Table "<<table_name<<" has field name empty . You MUST provide a field name"<<endl;
                          
                          return -1;
                        }
                      if(verbose)  cout<<"table field name="<<table_field_name<<endl; 
                     
                    }
                      if (table_attribute_name=="type") {
                        table_field_type=my_utf8(xml_node_single_table_field.child("dia:string").child_value());
                        table_field_type  = table_field_type.substr(1, table_field_type.length()-2);
                        if (table_field_type.empty()) {
                        cerr<<"Error! Table "<<table_name<<" has field type is empty in field "<<table_field_name<<". You MUST provide a field  type"<<endl;
                        
                        return -2;
                        }
                     if(verbose)    cout<<"table field type="<<table_field_type<<endl; 
                         
                      }
                         
                     if (table_attribute_name=="comment") {
                       table_field_comment=my_utf8(xml_node_single_table_field.child("dia:string").child_value());
                       table_field_comment  = table_field_comment.substr(1, table_field_comment.length()-2);
                       table_field_comment="comment '"+table_field_comment+"'";
                   if(verbose)      cout<<"table field comment="<<table_field_comment<<endl; 
                     };
                      if (table_attribute_name== "primary_key"){
                        table_field_is_primary_key=my_utf8(xml_node_single_table_field.child("dia:boolean").attribute("val").value());
                    if(verbose)     cout<<"table field is primary_key? "<<table_field_is_primary_key<<endl;
                         if (table_field_is_primary_key=="true") primary_key_predicate+=table_field_name+",";
                      }
                       if (table_attribute_name=="nullable") {
                      table_field_is_nullable=my_utf8(xml_node_single_table_field.child("dia:boolean").attribute("val").value());
                   if(verbose)    cout<<"table field nullable? "<<table_field_is_nullable<<endl; 
                      
                         
                      }
                     if (table_attribute_name=="unique") {
                      table_field_is_unique=my_utf8(xml_node_single_table_field.child("dia:boolean").attribute("val").value());
                    if(verbose)   cout<<"table field is unique? "<<table_field_is_unique<<endl; 
                       if (table_field_is_unique=="true") unique_predicate+=" "+table_field_name+",";
                      }
                      if (table_attribute_name=="default_value") {
                        table_field_default_value=my_utf8(xml_node_single_table_field.child("dia:string").child_value());
                        table_field_default_value  = table_field_default_value.substr(1, table_field_default_value.length()-2);
                      if(verbose)   cout<<"table field default_value ="<<table_field_default_value<<endl; 
                      }
                         
                  }
                  
                   if(table_field_is_nullable=="true") default_value_predicate="default null";
                   if(table_field_is_nullable=="false") default_value_predicate="default not null";
                   if (table_field_default_value.length()>0) default_value_predicate="default " + table_field_default_value;
                    
                   
                  table_field_definitions_list_predicate+=" "+table_field_name+" " + table_field_type+" "+default_value_predicate+" "+table_field_comment+",";
                }
                if(primary_key_predicate.length()>0) {
                    primary_key_predicate=", PRIMARY KEY("+primary_key_predicate.substr(0,primary_key_predicate.length()-1)+")";
                  }
                  if(unique_predicate.length()>0) {
                    unique_predicate="Unique("+unique_predicate.substr(0,unique_predicate.length()-1)+")";
                  }
                
               }
             
        }  
        
        if(unique_predicate.length()>0) {
          primary_key_predicate+=","+unique_predicate; //put the unique_predicate after the primary_key_predicate with a comma between them 
        } 
        table_field_definitions_list_predicate[table_field_definitions_list_predicate.length()-1]=' '; // the last comma in the table_field_definitions_list_predicate must be removed 
        all_sql_statements+=sql_create_table_predicate+table_field_definitions_list_predicate+primary_key_predicate+");\n";
      
        primary_key_predicate="";
        unique_predicate="";
        table_field_definitions_list_predicate="";
      
    }
   if (my_utf8(xml_node_all_diagram_objects.attribute("type").value()) =="Database - Reference") {
    
    for (xml_node xml_node_list_of_fk = xml_node_all_diagram_objects.child("dia:attribute"); xml_node_list_of_fk; xml_node_list_of_fk = xml_node_list_of_fk.next_sibling()){
    if (my_utf8(xml_node_list_of_fk.attribute("name").value())=="start_point_desc")  cout<<"start_point_desc: "<<xml_node_list_of_fk.child("dia:string").child_value()<<endl;
    if (my_utf8(xml_node_list_of_fk.attribute("name").value())=="end_point_desc") cout<<"end_point_desc: "<<xml_node_list_of_fk.child("dia:string").child_value()<<endl;
      
    }

     single_connection.connection_id_from_dia_object_id=my_utf8(xml_node_all_diagram_objects.attribute("id").value());
     for (xml_node xml_node_list_of_references = xml_node_all_diagram_objects.first_element_by_path("dia:connections/dia:connection"); xml_node_list_of_references; xml_node_list_of_references = xml_node_list_of_references.next_sibling()){
       
      if (my_utf8(xml_node_list_of_references.attribute("handle").value())=="0") single_connection.handle_zero_connection_object_id=my_utf8(xml_node_list_of_references.attribute("to").value());
      if (my_utf8(xml_node_list_of_references.attribute("handle").value())=="1") single_connection.handle_one_connection_object_id=my_utf8(xml_node_list_of_references.attribute("to").value());
       
    }
    connections_list.push_back(single_connection);
   }
      }
      
      for (auto it = connections_list.begin();it!=connections_list.end();it++){
           cout<<"connection object id="<< it->connection_id_from_dia_object_id << " handle_0="<<map_dia_object_id_to_table_name[it->handle_zero_connection_object_id]<<" handle_one="<<map_dia_object_id_to_table_name[ it->handle_one_connection_object_id]<<endl;
           
      }
      
      if (output_file_path.length()>0) {
        ofstream outfile (output_file_path);
        if (outfile.is_open()){
          outfile<<"--  Generated SQL CODE:\n"<<all_sql_statements<<endl;
          outfile.close();
        } else {
          cerr<<"cannot open file "<<output_file_path<<" for saving Generated SQL Statements "<< endl;
        }
      } else {
      
      cout<<"--  Generated SQL CODE:\n"<<all_sql_statements<<endl;
      }
    return 0;
}
