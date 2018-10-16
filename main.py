import sys
from PyQt5.QtWidgets import (QWidget, QDesktopWidget, QPushButton, QLineEdit, QTableWidget, QTableWidgetItem, \
                             QLabel, QDialog, QHeaderView, QHBoxLayout, QVBoxLayout, QGridLayout, QMessageBox, \
                             QApplication)
import random
from PyQt5.QtGui import QIcon

class Dict(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):

        self.addButton = QPushButton("add")
        self.editButton = QPushButton("edit")
        self.mixButton = QPushButton("mix up")
        self.infoButton = QPushButton("info")
        self.line = QLineEdit()
        self.label = QLabel("search:")

        self.table = QTableWidget()
        self.table.setColumnCount(2)
        self.table.horizontalHeader().setSectionResizeMode(1, QHeaderView.Stretch)
        self.table.setHorizontalHeaderItem(0, QTableWidgetItem("word"))
        self.table.setHorizontalHeaderItem(1, QTableWidgetItem("translate"))

        hbox1 = QHBoxLayout()
        hbox1.addWidget(self.addButton)
        hbox1.addWidget(self.editButton)
        hbox1.addWidget(self.mixButton)
        hbox1.addWidget(self.infoButton)

        hbox2 = QHBoxLayout()
        hbox2.addWidget(self.label)
        hbox2.addWidget(self.line)

        vbox = QVBoxLayout()
        vbox.addLayout(hbox1)
        vbox.addLayout(hbox2)
        vbox.addWidget(self.table)

        self.setLayout(vbox)

        self.line.textChanged.connect(self.findWord)
        self.infoButton.pressed.connect(self.showInfo)
        self.addButton.pressed.connect(self.wordInput)
        self.editButton.pressed.connect(self.editWord)
        self.mixButton.pressed.connect(self.mixFunc)
        self.table.horizontalHeader().sectionClicked.connect(self.table.sortByColumn)


        self.setGeometry(QDesktopWidget().availableGeometry().width() / 4, \
                         QDesktopWidget().availableGeometry().height() / 4, \
                         QDesktopWidget().availableGeometry().width() / 2, \
                         QDesktopWidget().availableGeometry().height() / 2)
        self.setWindowTitle('dictionary')
        self.setWindowIcon(QIcon('icoDict.ico'))

        self.show()

        self.emptyFlag = True

        try:
            with open("dict.txt") as file:
                i = 0
                for str in file:
                    self.table.insertRow(i)
                    self.table.setItem(i, 0, QTableWidgetItem(str.strip('\n')))
                    self.table.setItem(i, 1, QTableWidgetItem(file.readline().strip('\n')))
                    i += 1
        except:
            open("dict.txt", "w").close()
            self.emptyFlag = False
            QMessageBox.warning(self, "warning", "Dictionary file not found.\n\
New dictionary file created.")

    def findWord(self):
        word = self.line.text()

        self.table.clearContents()
        self.table.setRowCount(0)

        i = 0

        if word == "":
            with open("dict.txt") as file:
                for str in file:
                    self.table.insertRow(i)
                    self.table.setItem(i, 0, QTableWidgetItem(str.strip('\n')))
                    self.table.setItem(i, 1, QTableWidgetItem(file.readline().strip('\n')))
                    i += 1
        else:
            with open("dict.txt") as file:
                for str in file:
                    if str.find(word) != -1:
                        self.table.insertRow(i)
                        self.table.setItem(i, 0, QTableWidgetItem(str.strip('\n')))
                        self.table.setItem(i, 1, QTableWidgetItem(file.readline().strip('\n')))
                        i += 1
                    else:
                        s = file.readline()
                        if s.find(word) != -1:
                            self.table.insertRow(i)
                            self.table.setItem(i, 0, QTableWidgetItem(str.strip('\n')))
                            self.table.setItem(i, 1, QTableWidgetItem(s.strip('\n')))
                            i += 1

    def showInfo(self):
        QMessageBox.information(self, "info", "Search - input word and press Enter.\nLoad all words - input clear \
string and press Enter.\nAdd - press add button, input eng word, input translate and press Enter.\nEdit - edit \
data in the table and press edit (Only words in table will be wrote!).\nMix up - mix up words in table.")

    def wordInput(self):
        dialog = QDialog(self)

        dialog.dline1 = QLineEdit()
        dialog.dlabel1 = QLabel("word:")
        dialog.dline2 = QLineEdit()
        dialog.dlabel2 = QLabel("translate:")

        dialog.dline1.returnPressed.connect(lambda: self.addWord(dialog))
        dialog.dline2.returnPressed.connect(lambda: self.addWord(dialog))

        grid = QGridLayout()
        dialog.setLayout(grid)
        grid.addWidget(dialog.dlabel1, 0, 0)
        grid.addWidget(dialog.dline1, 0, 1)
        grid.addWidget(dialog.dlabel2, 1, 0)
        grid.addWidget(dialog.dline2, 1, 1)

        dialog.resize(self.geometry().width() * 5 / 6, 0)
        dialog.setFixedHeight(dialog.sizeHint().height())

        dialog.setWindowTitle('add')
        dialog.exec()

    def addWord(self, dialog):
        with open("dict.txt", "a") as file:
            if self.emptyFlag:
                file.write('\n' + dialog.dline1.text() + '\n' + dialog.dline2.text())
            else:
                file.write(dialog.dline1.text() + '\n' + dialog.dline2.text())
                self.emptyFlag = True

        self.table.clearContents()
        self.table.setRowCount(0)
        self.line.clear()

        i = 0

        with open("dict.txt") as file:
            for str in file:
                self.table.insertRow(i)
                self.table.setItem(i, 0, QTableWidgetItem(str.strip('\n')))
                self.table.setItem(i, 1, QTableWidgetItem(file.readline().strip('\n')))
                i += 1

        dialog.close()

    def editWord(self):
        with open("dict.txt", "w") as file:
            for i in range(self.table.rowCount()):
                if i:
                    file.write('\n' + self.table.item(i, 0).text() + '\n' + self.table.item(i, 1).text())
                else:
                    file.write(self.table.item(i, 0).text() + '\n' + self.table.item(i, 1).text())

    def mixFunc(self):
        for i in range(self.table.rowCount() - 1):
            rand = random.randint(i + 1, self.table.rowCount() - 1)

            swap = self.table.item(i, 0).text()
            self.table.setItem(i, 0, QTableWidgetItem(self.table.item(rand, 0).text()))
            self.table.setItem(rand, 0, QTableWidgetItem(swap))

            swap = self.table.item(i, 1).text()
            self.table.setItem(i, 1, QTableWidgetItem(self.table.item(rand, 1).text()))
            self.table.setItem(rand, 1, QTableWidgetItem(swap))

if __name__ == '__main__':
    app = QApplication(sys.argv)
    d = Dict()
    sys.exit(app.exec_())