#ifndef MSSQL_H_INCLUDED
#define MSSQL_H_INCLUDED


#include <fstream>
#include <string>

#include "nanodbc.h"
//#include <vector>
#include <wx/grid.h>
#include <wx/msgdlg.h>
#include <wx/mstream.h>
#include <wx/statbmp.h>

std::string INS_UPD_DEL_SQL_SERV_V2(std::string ,std::string);
bool SQL_SERV_GRID_V2 (std::string  ,wxGrid *  ,std::string );
std::vector <std::vector <std::string>> SQL_SERV_VEXT_V2(std::string SEL_STR, std::string CONN_STR,bool = false);
std::string SQL_SERV_SEL_V2 (std::string SEL_STR  ,std::string CONN_STR);
std::vector<std::uint8_t> read_varbinary_blob(std::string SEL_STR  ,std::string CONN_STR);
bool insert_varbinary_blob(std::string path_to_file,std::string sql_txt ,std::string CONN_STR);
void StaticBitmap_From_SQLDB(wxStaticBitmap * btm, std::string sql_txt , std::string  CONN_STR);
#endif // MSSQL_H_INCLUDED
