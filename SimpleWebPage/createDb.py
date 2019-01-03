import sqlite3
import md5
import os

DB_PATH =r"c:\facultate\PSNCS\ClientServerHW\SimpleWebPage\db.sqllite"

def connectToDb():
	connection = sqlite3.connect(DB_PATH)
	# connection.text_factory = lambda x: unicode(x, 'utf-8', 'ignore')
	return connection
			
def createTable(cursor):
	createUsernameTableQuery = 'CREATE TABLE if not exists users ( \
							id integer primary key autoincrement, \
							username text not null, \
							password text not null, \
							type text not null\
							)'
							
	createMessagesTableQuery = 'CREATE TABLE if not exists messages ( \
							id integer primary key autoincrement, \
							username text not null, \
							content text not null \
							)' 
	
	result = cursor.execute(createUsernameTableQuery).fetchall()
	print "create username: " + str(result)
	

def insertUser(cursor, user, password, type):
	# m = md5.new()
	# m.update(password)
	# password = str(m.digest())
	
	# insertUsersQuery = ' INSERT INTO users (username,password,type) VALUES("' + str(user) + '","' + str(password) + '","' + str(type) + '")'
	insertUsersQuery = ' INSERT INTO users (username,password,type) VALUES(?,?,?)'
	print insertUsersQuery
	result = cursor.execute(str(insertUsersQuery), (str(user), sqlite3.Binary(buffer(password)), str(type))).fetchall()
	print "insert result: " + str(result)
	
def getAllFromTable(cursor, tableName):
	selectQuery = 'SELECT * FROM ' + str(tableName)
	result = cursor.execute(selectQuery).fetchall()
	print str(result)
	
def populateUsers(cursor):
	insertUser(cursor, 'admin','admin','admin')
	insertUser(cursor, 'admin2','admin2','admin')
	insertUser(cursor, 'ion','ion','normal')
	insertUser(cursor, 'pop','popescu','normal')
	insertUser(cursor, 'test','test','normal')
	
	# users = [('admin','admin','admin'),
             # ('admin2','admin2','admin'),
             # ('ion','ion','normal'),
			 # ('pop','popescu','normal'),
			 # ('test','test','normal')
            # ]
	# insertedUsers = []		
	# for item in users:
		# m = md5.new()
		# m.update(item[1])
		# insertedUsers.append((item[0], str(m.digest()), item[2]))
	# print(str(insertedUsers))
	# cursor.executemany('INSERT INTO users (username,password,type) VALUES (?,?,?)', insertedUsers)
	
os.remove(DB_PATH)
connection = connectToDb()
cursor = connection.cursor()
createTable(cursor)
populateUsers(cursor)
connection.commit()
getAllFromTable(cursor, 'users')
connection.commit()