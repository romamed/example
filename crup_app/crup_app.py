import sys
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QDesktopWidget, QLineEdit, QLabel, QHBoxLayout, QVBoxLayout, \
    QTableWidget, QHeaderView, QTableWidgetItem
import mysql.connector
from mysql.connector import Error

class Crud(QWidget):
    host='localhost'
    database='crud_app'
    user='admin'
    password='password'
    
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):

        self.loadButton = QPushButton("load stores")
        self.insertButton = QPushButton("insert store")
        self.editButton = QPushButton("edit store")
        self.deleteButton = QPushButton("delete store")
        self.line = QLineEdit()
        
        self.table = QTableWidget()
        
        hbox1 = QHBoxLayout()
        hbox1.addWidget(self.loadButton)
        hbox1.addWidget(self.insertButton)
        hbox1.addWidget(self.editButton)
        hbox1.addWidget(self.deleteButton)
        
        vbox = QVBoxLayout()
        vbox.addLayout(hbox1)
        vbox.addWidget(self.line)
        vbox.addWidget(self.table)
        
        self.setLayout(vbox)
        
        self.setGeometry(QDesktopWidget().availableGeometry().width() / 4, \
                         QDesktopWidget().availableGeometry().height() / 4, \
                         QDesktopWidget().availableGeometry().width() / 2, \
                         QDesktopWidget().availableGeometry().height() / 2)
        self.setWindowTitle('crud_app')
        
        self.loadButton.pressed.connect(self.load_store)
        self.insertButton.pressed.connect(self.insert_store)
        self.editButton.pressed.connect(self.edit_store)
        self.deleteButton.pressed.connect(self.delete_store)
        
        self.load_store()
        
        self.show()
    
    def load_store(self):
        """ Read store table from MySQL database """
        try:
            conn = mysql.connector.connect(host=self.host,
                                           database=self.database,
                                           user=self.user,
                                           password=self.password)
        
        except Error as e:
            self.line.setText(str(e))
        else:
            try:
                cursor = conn.cursor()
                cursor.execute("SELECT * FROM store")

                row = cursor.fetchone()
                
                self.table.setColumnCount(3)
                self.table.horizontalHeader().setSectionResizeMode(2, QHeaderView.Stretch)
                self.table.setHorizontalHeaderItem(0, QTableWidgetItem("store"))
                self.table.setHorizontalHeaderItem(1, QTableWidgetItem("site"))
                self.table.setHorizontalHeaderItem(2, QTableWidgetItem("address"))
                
                self.table.clearContents()
                self.table.setRowCount(0)
                
                i=0
                while row is not None:
                    self.table.insertRow(i)
                    self.table.setItem(i, 0, QTableWidgetItem(row[1]))
                    self.table.setItem(i, 1, QTableWidgetItem(row[2]))
                    self.table.setItem(i, 2, QTableWidgetItem(row[3]))
                    i = i + 1
                    row = cursor.fetchone()

            except Error as e:
                self.line.setText(str(e))

            finally:
                cursor.close()
                conn.close()
    
    def insert_store(self):
        """ Insert string to store table """
        try:
            conn = mysql.connector.connect(host=self.host,
                                           database=self.database,
                                           user=self.user,
                                           password=self.password)
        except Error as e:
            self.line.setText(str(e))
        else:
            try:
                line = self.line.text()
                self.line.clear()
                line = line.split(' ', 2)
                query = f'INSERT INTO store (name, site, address) VALUES("{line[0]}","{line[1]}","{line[2]}")'
        
                cursor = conn.cursor()
                cursor.execute(query)
                conn.commit()
            except Error as e:
                self.line.setText(str(e))
            finally:
                cursor.close()
                conn.close()
    
    def edit_store(self):
        """ Edit string in store table """
        try:
            conn = mysql.connector.connect(host=self.host,
                                           database=self.database,
                                           user=self.user,
                                           password=self.password)
        except Error as e:
            self.line.setText(str(e))
        else:
            try:
                line = self.line.text()
                self.line.clear()
                line = line.split(' ', 3)
                query = f'UPDATE store SET name = "{line[1]}", site = "{line[2]}", address = "{line[3]}" WHERE id = {line[0]}'
        
                cursor = conn.cursor()
                cursor.execute(query)
                conn.commit()
            except Error as e:
                self.line.setText(str(e))
            finally:
                cursor.close()
                conn.close()
    
    def delete_store(self):
        """ Delete string in store table """
        try:
            conn = mysql.connector.connect(host=self.host,
                                           database=self.database,
                                           user=self.user,
                                           password=self.password)
        except Error as e:
            self.line.setText(str(e))
        else:
            try:
                line = self.line.text()
                self.line.clear()
                line = line.split(' ', 1)
                query = f'DELETE FROM store WHERE id = {line[0]}'
        
                cursor = conn.cursor()
                cursor.execute(query)
                conn.commit()
            except Error as e:
                self.line.setText(str(e))
            finally:
                cursor.close()
                conn.close()

if __name__ == '__main__':

    app = QApplication(sys.argv)

    w = Crud()

    sys.exit(app.exec_())
