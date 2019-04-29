# Telem Packet Editor Doc


### Usage
#### Loading
- From File menu click 'Load'
#### Saving
- From File menu click 'Save'



### General Info
- On init the SessionController creates a PacketView and PacketController.
- PacketView and PacketController are created without a PacketModel. 

- SessionController can load PacketModels into the PacketView. The PacketView loads the model into PacketController.
(It would be better for SessionController to load the model into PacketController.)

- Changing between PacketModels allows the user to create/edit multiple packets during a session.

###Classes

#### SessionController
- A 'session' represents the running application. 
- Handles the interaction between the SessionModel, PacketController, and PacketView.

#### SessionModel
- Holds data important to the application. Namely PacketModels.

#### PacketController
-  Handles the interaction between the PacketView and the loaded PacketModel. 
-  Handles the loading and saving of PacketModels into csv files. 

#### PacketModel
- PacketModels can be created and loaded into the PacketView and PacketController.

#### PacketView
- Represents the application UI.


## CSV Formating
- Save and Load are handled in PacketController.


