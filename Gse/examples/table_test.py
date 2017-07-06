
import time
import Tkinter
from tkintertable.Tables import TableCanvas
from tkintertable.TableModels import TableModel

master = Tkinter.Tk()
tframe = Tkinter.Frame(master)
tframe.pack()
table = TableCanvas(tframe)
table.createTableFrame()

# Create a model from data and init.
model = TableModel()
table = TableCanvas(tframe, model=model,width=1200, height=800)
table.createTableFrame()
raw_input('Redraw table and put data into it!')
# Call to update after content change.
table.redrawTable()

#To import from a dictionary we get a handle on the model 
#(or we can create the model first and supply it as an argument to the table constructor):

data = {'rec1': {'col1': 99.88, 'col2': 108.79, 'label': 'rec1'},
'rec2': {'col1': 99.88, 'col2': 108.79, 'label': 'rec2'},
'rec3': {'col1':1.0, 'label':'rec3'},
'rec4': {'col1':2.0, 'label':'rec4'}
}

model = table.model
model.importDict(data) #can import from a dictionary to populate model
table.redrawTable()


#by column name
raw_input('Sort table by column name')
table.sortTable(columnName='label')

#by column index
raw_input('Sort table by column index')
table.sortTable(columnIndex=1) 

raw_input('Add automatic key row')
#add with automatic key
table.addRow()

raw_input('Add row with new key name')
#add with named key, other keyword arguments are interpreted as column values
keyname = 'rec5'
table.addRow(keyname, label='abc')

raw_input('Add column')
colname = 'new column'
table.addColumn(colname)

raw_input('Remove row')
#delete rows
table.deleteRow()

raw_input('Put value in (2,3) cell...')
row='rec4'
col='col2'
value=-999999.0
table.model.data[row][col] = value
#
for d in table.model.data:
    print d

table.redrawTable()

raw_input('Demo a cell value changing')
row='rec1'
col='col2'
for i in range(100):
    table.model.data[row][col] = i
    table.redrawTable()
    master.update()
    print i
    time.sleep(0.1)

raw_input('Change column heading')
# Column labels can be changed programmatically by accessing the columnlabels
# attribute of the table model:
table.model.columnlabels[colname] = "old column"
table.redrawTable()

raw_input('Quit')
master.destroy()

master.mainloop()


