package gov.nasa.jpl.componentaction;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import javax.swing.JFileChooser;

import com.nomagic.magicdraw.core.Application;
import com.nomagic.magicdraw.core.Project;
import com.nomagic.uml2.ext.jmi.helpers.ModelHelper;
import com.nomagic.uml2.ext.jmi.helpers.StereotypesHelper;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Element;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.NamedElement;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Property;
import com.nomagic.uml2.ext.magicdraw.mdprofiles.Stereotype;

/**
 * 
 * LoadIDConfig allows the user to specify a config CSV that will modify all the baseIDs and window ranges within the model. 
 * This allows users to have isfgen generate IDs and use them in the future by adding them into the models. 
 * 
 * Exceptions are generated when anything does not conform to what is expected, including file types, file column amounts, and mismatches between the config file and the model.
 * 
 * <p>
 * 
 * The format of the config file is so:
 * <p>
 * component,instance,base_id,base_window_range
 * <p>
 * SignalGen,SG1,10,15
 * <p>
 * ...
 * <p>
 * 
 */
public class LoadIDConfig {
	private static Project project;

	/**
	 * Used to store information for each configuration item from the config file.
	 *
	 */
	public static class configItem{
		
		String componentType;
		String instanceName;
		int baseID;
		int windowRange;
		
		public configItem(String compType , String instName , int bID , int wRange){
			componentType = compType;
			instanceName = instName;
			baseID = bID;
			windowRange  = wRange;
		}

		@Override
		public int hashCode() {
			final int prime = 31;
			int result = 1;
			result = prime * result + baseID;
			result = prime * result + ((componentType == null) ? 0 : componentType.hashCode());
			result = prime * result + ((instanceName == null) ? 0 : instanceName.hashCode());
			result = prime * result + windowRange;
			return result;
		}

		@Override
		public boolean equals(Object obj) {
			if (this == obj)
				return true;
			if (obj == null)
				return false;
			if (getClass() != obj.getClass())
				return false;
			configItem other = (configItem) obj;
			if (baseID != other.baseID)
				return false;
			if (componentType == null) {
				if (other.componentType != null)
					return false;
			} else if (!componentType.equals(other.componentType))
				return false;
			if (instanceName == null) {
				if (other.instanceName != null)
					return false;
			} else if (!instanceName.equals(other.instanceName))
				return false;
			if (windowRange != other.windowRange)
				return false;
			return true;
		}

		@Override
		public String toString() {
			return "configItem [componentType=" + componentType + ", instanceName=" + instanceName + ", baseID="
					+ baseID + ", windowRange=" + windowRange + "]";
		}

	}
	
	private LoadIDConfig() { }

	/**
	 * process handles all the phases of this methodology. 
	 * 
	 * @param proj Project object
	 * @param pluginDir Directory to generate documents into
	 * @throws IOException
	 */
	public static void process(Project proj, File pluginDir) throws IOException {
		project = proj;

		String fileName = null;
		
		try {
			if (project == null) {
				Utils.printToAll("***No model found");
			} 
			else {
				Utils.printToAll("Project Name = " + project.getName());
				File projectFile = new File(project.getFileName());
				
				//Phase One: Creating dialog box to retrieve input ID config file
				
				File configFile = getConfigFile(projectFile);
				
				//Phase Two: Validate and process the configFile
				
				Map<String , configItem> configMap  = validateAndProcessConfig(configFile);
				
				//Phase Three: Parse model and make changes to objects
				
				Element root = project.getModel();
				changeModel(root , proj ,  configMap);
				

			}	
				
		} 
		catch (LoadIDException e) {
			Application.getInstance().getGUILog()
					.log("<font color=\"#FF0000\">===> FATAL Exception: " + e.getMessage() + "</font>");
			throw new RuntimeException(e.getMessage());
		}

	}
	
	/**
	 * This function goes through the model and matches each component with its respective instance within the configMap. 
	 * 
	 * If there isn't any discrepancies between the model and the config files, baseIDs and window ranges are set to the values from the config file. 
	 * 
	 * All subsystems have their base IDs set to zero.
	 * 
	 * @param root
	 * @param proj
	 * @param configMap
	 * @throws LoadIDException
	 */
	public static void changeModel(Element root , Project proj ,  Map<String , configItem> configMap) throws LoadIDException{
		Collection<Element> collection = (Collection<Element>)ModelHelper.getElementsOfType(root, null, true, true);
		ArrayList<Element> componentList = new ArrayList<Element>();
		Stereotype componentInstanceStereotype = StereotypesHelper.getStereotype(proj, "ComponentInstance");
		
		//Make set of all instance names. This will be used to see if all instances are used
		Set<String> instanceNames = new HashSet<String>();
		for(String key : configMap.keySet()){
			instanceNames.add(key);
		}
		
		//Iterate through all the elements in the model
		for(Element el : collection) {
			if(el instanceof NamedElement) {
				NamedElement elNamed  = (NamedElement) el;
				if (el.getHumanType().equals("Part Property") || el.getHumanType().equals("Reference Property") ){
					Property elProp = (Property) el;
					//If the element is a component set both window and base ID
					if(elProp.getType() == null){
						throw new LoadIDException("Component " + elNamed.getName() + " is found in the SysML model and is of type " + el.getHumanType() + " but doesn't have an explicit type object.");
					}
					else if(elProp.getType().getHumanType() == null){
						//throw new LoadIDException("Component " + elNamed.getName() + " is found in the SysML model and is of type " + el.getHumanType() + " but doesn't have a human type of the type.");
					}
					else if(!elProp.getType().getHumanType().equalsIgnoreCase("Subsystem")){
						if(configMap.containsKey(elNamed.getName())){		
							componentList.add(el);
							instanceNames.remove(elNamed.getName());
						}
						else{
							//throw new LoadIDException("Component " + elNamed.getName()  + " is found in the SysML model but not the config file. Make sure the config file matches with this model.");
						}
					}
				}
				//If the element is a subsystem, set the base ID to zero
				else if (el.getHumanType().equals("Subsystem")) {
					StereotypesHelper.setStereotypePropertyValue(el , componentInstanceStereotype, "BaseID" , 0);
				}
			}
		}
		
		if(instanceNames.size() > 0){
			boolean first = true;
			String errorString = "";
			for(String s : instanceNames){
				if(!first){
					errorString += ',';
				}
				else{
					first = false;
				}
				errorString += s;
			}
			throw new LoadIDException("Components " + errorString + " are found in the config file but not in the SysML model. Make sure the config file matches with this model." );
		}
		
		//Set base IDs of subsystems to zero. This wasn't done in the earlier loop to ensure the model and files are valid before any changes are made
		for(Element el : collection) {
			if(el instanceof NamedElement) {
				if (el.getHumanType().equals("Subsystem")) {
					StereotypesHelper.setStereotypePropertyValue(el , componentInstanceStereotype, "BaseID" , 0);
				}
			}
		}
		
		//Change base IDs and Window sizes for all the elements
		for(Element el : componentList){
			NamedElement elNamed  = (NamedElement) el;
			
			//Check if current component had previous values assigned to the base ID and window id stereotype values. If so, a override warning message is printed to the console
			Object baseIdObject = StereotypesHelper.getStereotypePropertyFirst(el, componentInstanceStereotype, "BaseID");
			Object windowRangeObject = StereotypesHelper.getStereotypePropertyFirst(el, componentInstanceStereotype, "Window");
			if(baseIdObject != null){
				if(! baseIdObject.toString().equals(((Integer) configMap.get(elNamed.getName()).baseID).toString()) ){
					Utils.printToAll("ValueOverrideWarning: Base ID for component " + elNamed.getName() + " was previously set to " + baseIdObject.toString() + " is being changed to " + ((Integer) configMap.get(elNamed.getName()).baseID).toString() + ".");
				}
			}
			if(windowRangeObject != null){
				if(! windowRangeObject.toString().equals(((Integer) configMap.get(elNamed.getName()).windowRange).toString()) ){
					Utils.printToAll("ValueOverrideWarning: Window range for component " + elNamed.getName() + " was previously set to " + windowRangeObject.toString() + " is being changed to " + ((Integer) configMap.get(elNamed.getName()).windowRange).toString() + ".");
				}
			}
			StereotypesHelper.setStereotypePropertyValue(el , componentInstanceStereotype, "BaseID" , configMap.get(elNamed.getName()).baseID);
			StereotypesHelper.setStereotypePropertyValue(el , componentInstanceStereotype, "Window" , configMap.get(elNamed.getName()).windowRange);
		}
		
	}
	
	
	/**
	 * Validates and processes the input config file.
	 * 
	 * Checks if the file is valid, is a .CSV, and has four items per row. 
	 * 
	 * This also constructs a map where the key is an instance name and the value is a configItem that holds the associated names and IDs.
	 * 
	 * @param config
	 * @return
	 * @throws LoadIDException
	 */
	public static Map<String , configItem> validateAndProcessConfig(File config) throws LoadIDException{
		Map<String , configItem> configMap = new HashMap<String , configItem>();
		String path = config.getAbsolutePath();
		
		//Validate File name
		//Check if valid file
		if(!config.isFile()){
			throw new LoadIDException("Chosen item " + path + " is not a file.");
		}
		//Check if readable
		if(!config.canRead()){
			throw new LoadIDException("Chosen file " + path + " is not readable.");
		}
		//Check if is a .csv file
		if(path.length() < 5){
			throw new LoadIDException("Chosen file " + path + " is not of type CSV.");
		}
		if(!path.substring(path.length() - 4 , path.length()).equalsIgnoreCase(".csv")){
			throw new LoadIDException("Chosen file " + path + " is not of type CSV.");
		}
		
		//Process and validate file contents
		
		try{
			BufferedReader configRead = new BufferedReader(new FileReader(config));
			String line;
			int lineNumber = 0;
			while((line = configRead.readLine()) != null){
				String[] splitLine = line.split(",");
				//Check if each line has four items
				if(splitLine.length != 4){
					throw new LoadIDException("Chosen file " + path + " has line " + line + " that does not only have four items.");
				}
				String compType = splitLine[0];
				String instanceName = splitLine[1];
				Integer baseID = null;
				Integer windowRange = null;
				
				//Checks if the base ID / window range can be parsed to an integer
				try{
					baseID = Integer.parseInt(splitLine[2]); 
					windowRange = Integer.parseInt(splitLine[3]); 
				}
				catch(Exception e){
					if(lineNumber > 0){
						Utils.notifyWarning("Line number " + lineNumber + " in file " + path + " has items in columns three and/or four that cannot be parsed to an integer.");
					}
				}
				
				
				//If all the config items are found and valid, add it to the dictionary
				if(baseID != null && windowRange != null){
					configItem newItem = new configItem(compType, instanceName, baseID, windowRange);
					
					//Check if instance name is in the map already
					if(configMap.containsKey(instanceName)){
						throw new LoadIDException("Instance of name " + instanceName + " has been found multiple times in the config file. Please make sure that instance names do not repeat.");
					}
					
					configMap.put(instanceName, newItem);
				}
				
				lineNumber++;
			}
		}
		catch(Exception e){
			throw new LoadIDException(e.getMessage());
		}
		
		return configMap;
	}
	
	/**
	 * Opens up a file chooser box for the user to select what config CSV file to choose.
	 * 
	 * The input argument projectFile is used to specify a starting path.
	 * 
	 * @param projectFile
	 * @return A config file object
	 * @throws LoadIDException
	 */
	public static File getConfigFile(File projectFile) throws LoadIDException{
		File outFile;
		JFileChooser configChooser = new JFileChooser();
		configChooser.setCurrentDirectory(projectFile);
		int result = configChooser.showOpenDialog(null);
		if(result == JFileChooser.APPROVE_OPTION){
			outFile = configChooser.getSelectedFile();
		}
		else{
			throw new LoadIDException("No file was selected!");
		}
		return(outFile);
	}

}