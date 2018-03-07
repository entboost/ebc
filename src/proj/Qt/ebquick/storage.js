.import QtQuick.LocalStorage 2.0 as Sql

// 首先创建一个helper方法连接数据库
var db = null;
function getDatabase() {
    if (db==null) {
        db = Sql.LocalStorage.openDatabaseSync("entboost", "1.0", "StorageDatabase", 100000);
    }
    return db;
}

// 程序打开时，初始化表
function initialize() {
    db = getDatabase();
    db.transaction(function(tx) {
        // 如果setting表不存在，则创建一个
        // 如果表存在，则跳过此步
        tx.executeSql('CREATE TABLE IF NOT EXISTS user_login_record_t(user_id BIGINT,account TEXT);');
    });
}

// 插入数据
function setSetting(userId, account) {
    db = getDatabase();
    var res = "";
    db.transaction(function(tx) {
        var rs = tx.executeSql('INSERT OR REPLACE INTO user_login_record_t(user_id,account) VALUES (?,?);', [userId,account]);
        //console.log(rs.rowsAffected)
        if (rs.rowsAffected > 0) {
            res = "OK";
        }
        else {
            res = "Error";
        }
    });
    return res;
}

// 获取数据
function getSetting(userId) {
    db = getDatabase();
    var res="";
    db.transaction(function(tx) {
        var rs = tx.executeSql('SELECT account FROM user_login_record_t WHERE user_id=?;', [userId]);
        if (rs.rows.length > 0) {
            res = rs.rows.item(0).value;
        } else {
            res = "Unknown";
        }
    });
    return res
}
