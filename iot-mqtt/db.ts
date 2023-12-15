import { Db, MongoClient } from "mongodb";

const config = {
  mongo: {
    url: "mongodb://iot-node-red:simplepassword@ac-hgit8ry-shard-00-00.je5vjkn.mongodb.net:27017,ac-hgit8ry-shard-00-01.je5vjkn.mongodb.net:27017,ac-hgit8ry-shard-00-02.je5vjkn.mongodb.net:27017/?ssl=true&replicaSet=atlas-7ghq0l-shard-0&authSource=admin&retryWrites=true&w=majority",
    dbName: "test",
  },
};

class MongoConnection {
  conn: string;
  dbName: string;
  db!: Db;
  hasInit: boolean = false

  constructor(conn: string, dbName: string) {
    this.conn = conn;
    this.dbName = dbName;
  }

initMongo = async () => {
    // Connect using mongodb driver
    try {
      const client = await MongoClient.connect(this.conn);
      this.db = client.db(this.dbName);
      this.hasInit = true
      return this.db;
    } catch (err) {
      console.log(err);
      throw err;
    }
  };
}

let _mongoConn: MongoConnection = new MongoConnection(config.mongo.url, config.mongo.dbName);
const mongo = async () => {
  try {
    if (_mongoConn.hasInit) {
        return _mongoConn
    }
    await _mongoConn.initMongo();
    return _mongoConn;
  } catch (err) {
    throw err;
  }
};

export default mongo;
