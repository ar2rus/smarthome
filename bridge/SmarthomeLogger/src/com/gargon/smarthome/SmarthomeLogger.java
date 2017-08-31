package com.gargon.smarthome;

import com.gargon.smarthome.clunet.ClunetDictionary;
import com.gargon.smarthome.logger.LoggerController;
import com.gargon.smarthome.logger.ConfigReader;
import com.gargon.smarthome.logger.RealTimeSupradinDataMessage;
import com.gargon.smarthome.logger.listeners.LoggerControllerMessageListener;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.json.JSONObject;

/**
 *
 * @author gargon
 */
public class SmarthomeLogger {

    private static final Logger LOG = Logger.getLogger(SmarthomeLogger.class.getName());
    
    // JDBC driver name and database URL
    private static final String JDBC_DRIVER = "com.mysql.jdbc.Driver";
    private static final String DB_URL = "jdbc:mysql://localhost/smarthome?useUnicode=true&characterEncoding=utf8";
    private static final String DB_TABLE = "sniffs";

    //  Database credentials
    private static final String DB_USER = "smarthome";
    private static final String DB_PASS = "";

    //Queries
    private static final String INSERT_QUERY = "insert into %s (s_time, src, dst, cmd, data, interpretation) values (?, ?, ?, ?, ?, ?)";
    
    private static LoggerController controller = null;

    private static Connection dbConnection = null;
    private static PreparedStatement preparedStmt = null;

    private static synchronized void log(RealTimeSupradinDataMessage message) {
        try {
            //System.out.println("inserting " + message.toString());
            preparedStmt.setLong(1, message.getTime());
            preparedStmt.setInt(2, message.getSrc());
            preparedStmt.setInt(3, message.getDst());
            preparedStmt.setInt(4, message.getCommand());
            preparedStmt.setBytes(5, message.getData());
            preparedStmt.setString(6, ClunetDictionary.toString(message.getCommand(), message.getData()));

            preparedStmt.execute();
        } catch (SQLException ex) {
            LOG.log(Level.SEVERE, "insert error on: " + message, ex);
        }
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        if (args.length == 1) {
            try {
                JSONObject config = ConfigReader.read(args[0]); //config.json

                JSONObject db = config.getJSONObject("db");
                
                Class.forName(JDBC_DRIVER);
                dbConnection = DriverManager.getConnection(db.optString("jdbc_url", DB_URL), db.optString("jdbc_user", DB_USER), db.optString("jdbc_pass", DB_PASS));
                dbConnection.setAutoCommit(true);
                preparedStmt = dbConnection.prepareStatement(String.format(INSERT_QUERY, db.optString("db_table", DB_TABLE)));

                controller = new LoggerController(config.optJSONObject("commands"));
                controller.addMessageListener(new LoggerControllerMessageListener() {

                    @Override
                    public void messages(List<RealTimeSupradinDataMessage> messages) {
                        for (RealTimeSupradinDataMessage m : messages) {
                            log(m);
                        }
                    }
                });

                Runtime.getRuntime().addShutdownHook(new Thread() {
                    public void run() {
                    
                        if (controller != null) {
                            controller.shutdown();
                        }

                        if (dbConnection != null) {
                            try {
                                dbConnection.close();
                            } catch (SQLException ex) {
                                LOG.log(Level.SEVERE, "connection closing error", ex);
                            }
                        }
                        
                       
                        LOG.log(Level.INFO, "SupradinKeeper closed");
                    }
                });

            } catch (Exception e) {
                LOG.log(Level.SEVERE, "initialization error", e);

                if (controller != null) {
                    controller.shutdown();
                }
                if (dbConnection != null) {
                    try {
                        dbConnection.close();
                    } catch (SQLException ex) {
                        LOG.log(Level.SEVERE, "connection closing error", ex);
                    }
                }
            }
        } else {
            LOG.log(Level.SEVERE, "Required path to config.json");
        }
    }

}