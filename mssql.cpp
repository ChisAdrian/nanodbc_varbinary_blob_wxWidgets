
#include "mssql.h"

void StaticBitmap_From_SQLDB(wxStaticBitmap * btm, std::string sql_txt , std::string  CONN_STR)
{
    auto imgdta = read_varbinary_blob(sql_txt,CONN_STR);
    unsigned char* jpegData = &imgdta[0];
    wxMemoryInputStream jpegStream(jpegData, imgdta.size());
    wxImage jpegImage;
    jpegImage.LoadFile(jpegStream, wxBITMAP_TYPE_JPEG);
    btm->SetBitmap(jpegImage);
}


std::string INS_UPD_DEL_SQL_SERV_V2(std::string EXE_Q,std::string CONN_STR)
{
    nanodbc::connection connection(CONN_STR);
    try
    {
        nanodbc::statement statement(connection);
        prepare(statement, EXE_Q);
        execute(statement);
        connection.disconnect();
    }
    catch(const std::exception& e)
    {
         connection.disconnect();
        return e.what();
    }

    //connection.disconnect();
    return "OK";
}

bool SQL_SERV_GRID_V2 (std::string SEL_STR ,wxGrid * grid ,std::string CONN_STR)
{

    nanodbc::result results;
    nanodbc::connection connection(CONN_STR);
    nanodbc::statement statement(connection);
  try {
        prepare(statement, SEL_STR);
        results = execute(statement);
        }
    catch(const std::exception& e)
    {
        wxMessageBox( e.what());
        return false;
    }

    grid->ClearGrid();
        if (grid->GetNumberRows()>0 ) grid->DeleteRows(0, grid->GetNumberRows());
        if (grid->GetNumberCols()>0 ) grid->DeleteCols(0, grid->GetNumberCols());
    grid->BeginBatch();

    int nrC = results.columns();
    grid->AppendCols(nrC);

     for(int c=0;c<nrC;c++)
         grid->SetColLabelValue(c,results.column_name(c));


    long rows_displayed = 0;

    wxArrayString column_datatype_names;
    for(short col = 0; col < nrC; ++col)
        column_datatype_names.Add(results.column_datatype_name(col));


    wxString displ_val;
    while(results.next())
    {
        grid->AppendRows(1);
        for(short col = 0; col < nrC; ++col)
        {
            if(column_datatype_names[col] == "float")
            {
                if(results.get<float>(col, 0) == results.get<int>(col, 0))
                    displ_val =  wxString::FromDouble(results.get<int>(col, 0),0);
                else
                    displ_val =  wxString::FromDouble(results.get<float>(col, 0),3);

            }
            else if(column_datatype_names[col]  == "int")
            {
                displ_val =  wxString::FromDouble(results.get<int>(col, 0),0);
            }

            else
            {
                displ_val = results.get<std::string>(col, "null");
            }

            grid->SetCellValue(rows_displayed,col,displ_val);
        }
        rows_displayed ++ ;
    }

    grid->EndBatch();
    grid->AutoSizeColumns();

   connection.disconnect();
return true;

}


std::vector <std::vector <std::string>>
SQL_SERV_VEXT_V2(std::string SEL_STR,std::string CONN_STR,bool col_header)
{
    std::vector <std::vector <std::string> > ret_ok;
    std::vector <std::vector <std::string>> errors_Catch;
    std::vector <std::string> errors_Catch_row;
    try
    {
        nanodbc::result results;
        nanodbc::connection connection(CONN_STR);
        nanodbc::statement statement(connection);
        prepare(statement, SEL_STR);
        results = execute(statement);
        int columns = results.columns();

        if(col_header == true)
        {
            std::vector <std::string> header_Vector ;
            for(short col = 0; col < columns; ++col)
                header_Vector.push_back( results.column_name(col));

            ret_ok.push_back(header_Vector);
        }


        while(results.next())
        {
            std::vector <std::string> rowVector ;
            for(short col = 0; col < columns; ++col)
                rowVector.push_back(results.get<std::string>(col, "null"));

            ret_ok.push_back(rowVector);
        }
    }
    catch(const std::exception& e)
    {
        //cerr << e.what() << endl;
        std::string for_errors_Catch = e.what();
        errors_Catch_row.push_back(for_errors_Catch);
        errors_Catch.push_back(errors_Catch_row);

        return errors_Catch;
    }

    return ret_ok;
}

std::string SQL_SERV_SEL_V2 (std::string SEL_STR  ,std::string CONN_STR)
{
    std::string ret_DATA="";
    nanodbc::result results;
    nanodbc::connection connection(CONN_STR);
    nanodbc::statement statement(connection);
    prepare(statement, SEL_STR);
    results = execute(statement);

    int nrC = results.columns();
    while(results.next())
    {

        for(short col = 0; col < nrC; ++col)
        ret_DATA += results.get<std::string>(col, "null");

    }

return ret_DATA;

}



std::vector<std::uint8_t> read_varbinary_blob(std::string SEL_STR  ,std::string CONN_STR)
{
    std::string ret_DATA="";
    nanodbc::result results;
    nanodbc::connection connection(CONN_STR);
    nanodbc::statement statement(connection);
    prepare(statement, SEL_STR);
    results = execute(statement);

    int nrC = results.columns();
    while(results.next())
    {
        return results.get<std::vector<std::uint8_t>>(0);
    }
}

bool insert_varbinary_blob(std::string path_to_file,std::string sql_txt ,std::string CONN_STR)
{
    std::ifstream file(path_to_file, std::ios::in | std::ios::binary);
    if (!file) {
      wxMessageBox("An error occurred opening the file\n");
        return false;
    }

     file.seekg(0, std::ifstream::end);
    std::streampos size = file.tellg();
    file.seekg(0);

    char* buffer = new char[size];
    file.read(buffer, size);
     std::vector<uint8_t> blob(buffer, buffer + size);

      try
    {
        nanodbc::connection connection(CONN_STR);
        nanodbc::statement stmt(connection);
        prepare(stmt, sql_txt); //NANODBC_TEXT("INSERT INTO test_large_blob (data) VALUES (?)")
        std::vector<std::vector<std::uint8_t>> rows = {blob};
        stmt.bind(0,rows);
        execute(stmt);
    }
    catch(const std::exception& e)
    {
         // cout << e.what() << endl;
          wxMessageBox(e.what());
          return false;
    }
    return true;

}
