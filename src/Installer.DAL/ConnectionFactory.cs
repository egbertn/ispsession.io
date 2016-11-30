using System;
using System.Collections.Generic;
using System.Data.OleDb;
using System.Linq;
using System.Text;
using System.Data.Common;
using System.Data;
using System.Data.SqlClient;

namespace ADCCure.Configurator.DAL
{
    public class ConnectionFactory
    {
        public static IDbConnection DbConnection(EnumDbTypes dbType, string connection)
        {
            switch (dbType)
            {
                    case EnumDbTypes.OleDb:
                    return new OleDbConnection(connection);
                case EnumDbTypes.SQLServer:
                    return new SqlConnection(connection);
                default:
                    throw new Exception("Db Connection type not handled");
            }
        }
    }
}
