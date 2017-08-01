/*
    Bodbclient is a example application to use the bodb database.
    Copyright (C) 2009-2010  Akee Yang <akee.yang@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// bodbclient.cpp : Defines the entry point for the console application.
//

//#include <bodb/bodb2.h>
#include "../bodb/bodb2.h"
using namespace bo;

#include <fstream>
#include <iostream>
#include <string>
#include <complex>

void PrintHelp(void)
{
	std::cout << "\n********************* App Help *********************\n";
	std::cout << "\thelp(h)\t\tPrint this help.\n";
	std::cout << "\texit(x)\t\tExit this Application.\n";
	std::cout << "\n*********************\n";
	std::cout << "\tEXEC [SQL_FILE]\t\tSQL file.\n";
	std::cout << "\t[SQL];\t\t\tCREATE,UPDATE,INSERT,SELECT...;\n";

}

void process(int ret, PRESULTSET resultset);

int main(int argc, const char* argv[])
{
	CBodbHandler::pointer bodbService = bodb_init();

	std::string sAccount;
	std::string sPassword;
	std::cout << "Account(Default 'system'):";
	std::getline(std::cin, sAccount);
	if (sAccount.empty())
		sAccount = "system";
	std::cout << "Password:";
	std::getline(std::cin, sPassword);

	bodbService->setaccount(sAccount, sPassword);

	PrintHelp();
	std::string sql = "";
	while (true)
	{
		if (sql.empty())
			std::cout << "BODB:";
		//else
		//	std::cout << "";

		std::string command;
		try
		{
			std::getline(std::cin, command);
			if (command.length() == 0)
			{
#ifdef WIN32
				//std::wcin.clear(std::ios_base::goodbit, true);
				std::cin.clear(std::ios_base::goodbit);
#else
				//std::wcin.clear(std::ios_base::goodbit, true);
#endif
				continue;
			}
		}catch (const std::exception & e)
		{
			std::cout << e.what() << std::endl;
			continue;
		}catch (...)
		{
			std::cout << "some exception" << std::endl;
			continue;
		}

		if (command.compare("exit") == 0 ||
			command.compare("x") == 0)
		{
			break;
		}else if (command.compare("help") == 0 ||
			command.compare("?") == 0 ||
			command.compare("h") == 0)
		{
			PrintHelp();
		}else
		{
			std::string::size_type find = command.find(';');
			if (find == std::string::npos)
			{
				sql.append(command);
				continue;
			}
		
			sql.append(command.substr(0, find));
			if (sql.substr(0, 4).compare("EXEC") == 0)
			{
				std::string sfile = sql.substr(5, sql.size()-5);
				sql = "";

				std::fstream fs;
				fs.open(sfile.c_str(), std::ios::in);
				if (fs.is_open())
				{
					fs.seekg(0, std::ios::end);
					std::fstream::pos_type posTotal = fs.tellg();
					fs.seekg(0, std::ios::beg);
					int size = posTotal;

					char * tempBuffer = new char[1024+1];
					while (!fs.eof() && size > 0)
					{
						fs.read(tempBuffer, 1024);
						sql.append(tempBuffer, size > 1024 ? 1024 : size);
						size -= 1024;
					}
					delete[] tempBuffer;
					fs.close();

					std::string::size_type offset = 0;

					while (true)
					{
						find = sql.find(';', offset);
						if (find == std::string::npos)
						{
							break;
						}

						std::string sSql = sql.substr(offset, find-offset);
						offset = find + 1;

						find = sSql.find("//");
						if (find != std::string::npos)
						{
							continue;
						}

						PRESULTSET resultset = 0;
						int ret = bodbService->execsql(sSql.c_str(), &resultset);
						process(ret, resultset);
					}
				}else
				{
					std::cout << "SQL file error!" << std::endl;
				}
			}else
			{
				PRESULTSET resultset = 0;
				int ret = bodbService->execsql(sql.c_str(), &resultset);
				process(ret, resultset);
				//bodb_exec(sql.c_str(), &resultset);
			}
			sql = "";
		}
	}

	bodb_exit(bodbService);
	return 0;
}

bool isStringType(FieldType fieldType)
{
	return fieldType >= FT_CHAR && fieldType <= FT_BLOB;
}

void process(int ret, PRESULTSET resultset)
{
	std::cout << ret << std::endl;
	if (resultset)
	{
		for (int lineIndex=0; lineIndex<resultset->rscount; lineIndex++)
		{
			std::cout << lineIndex << ":" << std::endl;
			for (short fieldIndex=0; fieldIndex<resultset->rsvalues[lineIndex]->fieldcount; fieldIndex++)
			{
				CFieldVariant varField(resultset->rsvalues[lineIndex]->fieldvalues[fieldIndex]);
				if (isStringType(varField.VARTYPE))
					std::cout << "'";
				std::cout << varField.getString().c_str();
				if (isStringType(varField.VARTYPE))
					std::cout << "'";
				std::cout << "  ";
			}
			std::cout << std::endl;
		}
		bodb_free(resultset);
	}
}
