from PyQt4 import QtSql, QtCore
from tableDeBase import TableDeBase

class TableVersions (TableDeBase):
	def __init__(self):
            TableDeBase.__init__(self,"Versions")
            self.setField(("id","nomVersion","commentaire"))
            self.setTypeField(('int','str','str'),('id',))
            self.cols=" (nomVersion, commentaire) "

        def createSqlTable(self):
            query=QtSql.QSqlQuery()
            texteQuery ="create table Versions(id integer primary key autoincrement, nomVersion varchar(10),"
            texteQuery+="commentaire varchar(30));"
            print "Creation de TableVersions : " , query.exec_(texteQuery)


        def remplit(self):
            self.insereLigneAutoId(('Salome7.2.0',''))
            self.insereLigneAutoId(('Salome7.3.0',''))
            self.insereLigneAutoId(('Salome7.4.0',''))

        def creeVersion(self,version,commentaire=""):
            self.insereLigneAutoId((version,commentaire))

        
        def chercheVersion(self,version):
            query=QtSql.QSqlQuery()
            version=str(version)
            if QtCore.QString(version).toInt()[1]==True :
               texteQuery ="select id, nomVersion from Versions where id = " + str(version) +";"
            else:
               texteQuery ="select id, nomVersion from Versions where nomVersion ='" + version +"' ;"
            query.exec_(texteQuery)
            nb=0
            while(query.next()):
               nb=nb+1
               id=query.value(0).toInt()[0]
               nom=query.value(1).toString()
            if nb != 1 : return 0, 0, ""
            return  1, id, nom

           

