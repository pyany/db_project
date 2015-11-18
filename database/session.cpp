#include "session.hpp"
#include "consts.hpp"

bool Session::open_connection() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(PATH_TO_DATABASE);

    if (!db.open()) {
        qDebug() << "Failed to connect";
        return false;
    } else {
        qDebug() << "Connected...";
        return true;
    }
}

void Session::close_connection() {
    db.close();
    db.removeDatabase(QSqlDatabase::defaultConnection);
    qDebug() << "Closed connection";
}

int Session::login(QString username, QString password) {
    if (open_connection()) {
        QSqlQuery loginQuery(db);
        loginQuery.prepare("SELECT COUNT(*) FROM Users WHERE username=? AND password=?");
        loginQuery.bindValue(0, username);
        loginQuery.bindValue(1, password);

        if (loginQuery.exec()) {
            int count = 0;

            if (loginQuery.next())
                count = loginQuery.value(0).toInt();

            if (count == 1)
                return 0;
            else
                return 1;

        } else {
            qDebug() << loginQuery.lastError();
            return -1;
        }
        close_connection();
    } else {
        qDebug() << "Couldn't connect to the database";
        return -1;
    }
}

QSqlQueryModel *Session::departments() {
    if (open_connection()) {
        QSqlQueryModel *departments_model = new QSqlQueryModel();
        QSqlQuery *query = new QSqlQuery(db);

        query->prepare("SELECT name FROM Departments UNION SELECT ''");
        query->exec();
        departments_model->setQuery(*query);

        qDebug() << "Loaded all departments";
        close_connection();
        return departments_model;
    } else {
        qDebug() << "Couldn't connect to the database";
        return NULL;
    }
}

QSqlQueryModel *Session::chairs(QString &department) {
    if (open_connection()) {
        QSqlQueryModel *chairs_model = new QSqlQueryModel();
        QSqlQuery *query = new QSqlQuery(db);

        query->prepare("SELECT c.name FROM Chairs AS c JOIN Departments AS d WHERE c.key == d.key AND d.name == ? UNION SELECT ''");
        query->bindValue(0, department);
        query->exec();
        chairs_model->setQuery(*query);

        qDebug() << "Loaded all chairs for " + department + " department";
        close_connection();
        return chairs_model;
    } else {
        qDebug() << "Couldn't connect to the database";
        return NULL;
    }
}

QSqlQueryModel *Session::table(QString &department, QString &chair) {
    if (open_connection()) {
        QSqlQueryModel *table_model = new QSqlQueryModel();
        QSqlQuery *query = new QSqlQuery(db);
        QString statement = "SELECT p.id, p.first_name, p.last_name, p.record, p.sex, c.name AS 'Chair', d.name AS 'Department' "
                            "FROM Professors AS p, Chairs AS c join Departments AS d, Roster AS r "
                            "WHERE p.id == r.id AND r.code == c.code AND c.key == d.key ";

        if (department == "") {
            query->prepare(statement);
        } else if (chair == "") {
            statement += "AND d.name == ?";
            query->prepare(statement);
            query->bindValue(0, department);
        } else {
            statement += "AND d.name == ? AND c.name == ?";
            query->prepare(statement);
            query->bindValue(0, department);
            query->bindValue(1, chair);
        }

        query->exec();
        table_model->setQuery(*query);

        close_connection();
        return table_model;
    } else {
        qDebug() << "Couldn't connect to the database";
        return NULL;
    }
}

QSqlQueryModel *Session::professors(QString &chair) {
    if (open_connection()) {
        QSqlQueryModel *prof_model = new QSqlQueryModel();
        QSqlQuery *query = new QSqlQuery(db);
        QString statement = "SELECT '' UNION SELECT 'id:' || p.id || ' ' || p.first_name || ' ' || p.last_name "
                            "FROM Professors AS p, Roster AS r, Chairs AS c  "
                            "WHERE p.id == r.id AND r.code == c.code AND c.name == ?";

        query->prepare(statement);
        query->bindValue(0, chair);

        query->exec();
        prof_model->setQuery(*query);

        close_connection();
        return prof_model;
    } else {
        qDebug() << "Couldn't connect to the database";
        return NULL;
    }
}

QSqlQuery *Session::department_info(QString &department) {
    if (open_connection()) {
        QSqlQuery *dep_query = new QSqlQuery(db);
        dep_query->prepare("SELECT * FROM Departments WHERE name == ?");
        dep_query->bindValue(0, department);
        dep_query->exec();
        dep_query->next();

        qDebug() << dep_query->lastQuery();
        return dep_query;
    } else {
        qDebug() << "Couldn't connect to the database";
        return NULL;
    }
}

QSqlQuery *Session::chair_info(QString &chair) {
    if (open_connection()) {
        QSqlQuery *chr_query = new QSqlQuery(db);
        chr_query->prepare("SELECT * FROM Chairs WHERE name == ?");
        chr_query->bindValue(0, chair);
        chr_query->exec();
        chr_query->next();

        qDebug() << chr_query->lastQuery();
        return chr_query;
    } else {
        qDebug() << "Couldn't connect to the database";
        return NULL;
    }
}

QSqlQuery *Session::prof_info(QString &professor) {
    if (open_connection()) {
        QSqlQuery *prof_query = new QSqlQuery(db);
        prof_query->prepare("SELECT first_name, last_name, record, sex FROM Professors WHERE id == ?");
        qDebug() << professor.split(" ")[0].toInt();
        prof_query->bindValue(0, professor.split(" ")[0].toInt());
        prof_query->exec();
        prof_query->next();

        qDebug() << prof_query->lastQuery();
        return prof_query;
    } else {
        qDebug() << "Couldn't connect to the database";
        return NULL;
    }
}