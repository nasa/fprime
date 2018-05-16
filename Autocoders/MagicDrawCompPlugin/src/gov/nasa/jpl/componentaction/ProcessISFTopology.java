package gov.nasa.jpl.componentaction;

import gov.nasa.jpl.componentaction.Utils.Pair;
import gov.nasa.jpl.componentaction.isfxmlwriter.IsfSubXmlWriter;

import java.io.File;
import java.io.IOException;
import java.net.ConnectException;
import java.nio.file.Files;
import java.util.*;

import com.nomagic.magicdraw.core.Application;
import com.nomagic.magicdraw.core.Project;
import com.nomagic.uml2.ext.jmi.helpers.ModelHelper;
import com.nomagic.uml2.ext.jmi.helpers.StereotypesHelper;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Element;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.EnumerationLiteral;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.NamedElement;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Property;
import com.nomagic.uml2.ext.magicdraw.compositestructures.mdinternalstructures.ConnectableElement;
import com.nomagic.uml2.ext.magicdraw.compositestructures.mdinternalstructures.Connector;
import com.nomagic.uml2.ext.magicdraw.compositestructures.mdinternalstructures.ConnectorEnd;
import com.nomagic.uml2.ext.magicdraw.compositestructures.mdports.Port;

/**
 * This singleton class will process an ISF Project.
 * <p>
 * There are three main phases of the processing:
 * <ul>
 * 		<li>
 * 			PHASE ONE: Processing the model
 * 			<ul>
 * 				<li>
 * 					A list of all subsystems is made.
 * 				</li>
 * 				<li>
 * 					A map of all the components is made.
 * 				</li>
 * 				<li>
 * 					A list of all the target ports is made.
 * 				</li>
 * 				<li>
 * 					A list of all the source ports is made.
 * 				</li>
 * 				<li>
 * 					A map of all the port ends with a list of associated connectors (one list going in, the other going out) is made.
 * 				</li>
 * 			</ul>
 * 		</li>
 * 		<li>
 * 			PHASE TWO: Creating connections
 * 			<ul> 
 * 				<li>
 * 					All full connections are found using each port in the source port list as a start point.
 * 				</li> 
 * 				<li>
 * 					Further connections are found using the map of port ends with their associated connectors.
 * 				</li>
 * 				<li>
 * 					Some connectors are ignored depending on the multiplicity of the previous source port. This multiplicity is recalculated for every connector.
 * 				</li>
 * 				<li>
 *					When reached a port that is not owned by a subsystem, the type is checked. If the source port and the target port type do not matched, that target port is ignored.
 * 				</li>
 * 				<li>
 * 					This process goes on for each source port until either the target port is found, all the associated connectors have been done being searched, or a time out variable is reached.
 * 				</li>
 * 				<li>
 * 					Each source and derived target connector ends are processed through the ISFSubsystem physicalConnectionType and added to a list.
 * 				</li>
 * 			</ul>
 * 		</li>
 * 		<li>
 * 			PHASE THREE: Checking ports and writing to XML
 * 			<ul>
 * 				<li>
 * 					A map is created with the name of generic modules being associated with a list of the ports on that module.
 * 				</li>
 * 				<li>
 * 					This map is cross checked with all the instances of the modules. If something in the map does not exist within an instance, the user gets notified by a warning.
 * 				</li>
 * 				<li>
 * 					An ISFSubsystem topologyModel object is created using the pysicalConnectionType list and the  map of all the components.
 * 				</li>
 * 				<li>
 * 					This object is passed to the IsfSubXmlWriter and is used to generate the flattened XML topology file. 
 * 				</li>
 * 			</ul>
 * 		</li>
 * </ul>
 * 
 * Please note that if you have more than one subsystem to be processed, make sure one of them has a stereotype with the name "Top". 
 * This subsystem will be used for the name of the topology xml. If not, an exception will be thrown.
 * <p>
 *  
 * Exceptions can be generated at any time in this process. 
 * 
 * 
 */
public class ProcessISFTopology {
	private static Project project;
	private static Set<String> subsystemNameSet;
	
	
	private static HashMap<String , List<ArrayList<Connector>>> subsystemMap;
	
	private static Set<ConnectorEnd> sourcePorts;
	private static Set<ConnectorEnd> targetPorts;
	public static HashSet<ISFSubsystem.physicalConnectionType> physicalConnectionSet; 
	
	public static HashMap<String , ISFSubsystem.componentType> componentMap;
	public static ArrayList<ISFSubsystem.physicalConnectionType> physicalConnectionList;
	public static Map<String , ArrayList<String[]>> subsystemOwners;
	public static Map<String , Integer> subsystemInstances;
	public static Map<String ,ArrayList<String>> uniqueSubsystemInstanceNames;
	

	private ProcessISFTopology() { }

	/**
	 * process handles all the phases of this methodology. 
	 * 
	 * @param proj Project object
	 * @param pluginDir Directory to generate documents into
	 * @throws IOException
	 */
	public static void process(Project proj, File pluginDir) throws IOException {
		project = proj;
		subsystemNameSet = new HashSet<String>();
		subsystemMap = new HashMap<String , List<ArrayList<Connector>>>();
		
		sourcePorts = new HashSet<ConnectorEnd>();
		targetPorts = new HashSet<ConnectorEnd>();
		physicalConnectionSet = new HashSet<ISFSubsystem.physicalConnectionType>();
		
		componentMap = new HashMap<String , ISFSubsystem.componentType>();
		physicalConnectionList = new  ArrayList<ISFSubsystem.physicalConnectionType>();
		subsystemInstances = new HashMap<String , Integer>();
		subsystemOwners = new HashMap<String ,  ArrayList<String[]>>();
		uniqueSubsystemInstanceNames = new HashMap<String , ArrayList<String>>();
		
		String fileName = null;
		
		try {
			if (project == null) {
				Utils.printToAll("***No model found");
			} 
			else {
				//PHASE ONE: PROCESSING THE MODEL
				Utils.printToAll("Project Name = " + project.getName());
				File projectFile = new File(project.getFileName());
				String projectDir = projectFile.getParent() + "/AutoXML";
				
				//If the project dir doesn't exist, create it
				File projectDirObj = new File(projectDir);
				if (!projectDirObj.isDirectory()){
					projectDirObj.mkdir();
				}
				
				// Get a list of all the Subsystem elements
				Element root = project.getModel();
				List<Element> subsystemElementList = processModel(root);
				
				if(subsystemElementList.size() != 0){
					int subsystemListSize = subsystemElementList.size();
					for (int x = 0; x < subsystemListSize; x++) {
						processSubsystem(subsystemElementList.get(x));
						//Try to see if the subsystemListSize is one or if the current subsystem has the stereotype 'Top'. If so, the name of the subsystem is used as the name of the Topology XML file. 
						if(subsystemListSize == 1 || StereotypesHelper.hasStereotype(subsystemElementList.get(x), "Top")){
							String[] fileNameList = subsystemElementList.get(x).getHumanName().split(" ");
							fileName = fileNameList[fileNameList.length-1];
						}
					}	
					
					//PHASE TWO: CREATING CONNECTIONS
					createConnections();
					
					//PHASE THREE: CHECKING PORTS AND WRITING TO XML
					checkPorts();
					
					//If not filename was found, an error is thrown.
					if(fileName == null){
						throw new ConnectorException("Filename is not specified. Please add the stereotype 'Top' to the top level subsystem or only use one subsystem in the diagram.");
					}
					
					physicalConnectionList = indexChecking(physicalConnectionSet);
					
					ISFSubsystem.topologyModel topModel = new ISFSubsystem.topologyModel(componentMap, physicalConnectionList);
					topModel.baseID = ISFSubsystem.getBaseId(root);
					topModel.instanceWindow = ISFSubsystem.getInstanceWindow(root);
					topModel.name = fileName;
					
					IsfSubXmlWriter.write(topModel, "Topology", fileName, projectDir, pluginDir);
				}
				else{
					Utils.notifyWarning("NoSubsystemsFound - No objects with the stereotype 'Subsystem' have been found. Topology XML will not be generated.");
				}
			}
		} 
		catch (ConnectorException e) {
			Application.getInstance().getGUILog()
					.log("<font color=\"#FF0000\">===> FATAL Exception: " + e.getMessage() + "</font>");
			throw new RuntimeException(e.getMessage());
		}

	}
	
	
	/**
	 * PHASE THREE FUNCTION
	 * <p>
	 * 
	 * This function serves two purposes. The first is to convert the physicalConnectionSet to a physicalConnectionList. The second is to perform index checking and correction on ports of the cmdReg/cmd or com/cmdResponse type.
	 * 
	 * <p>
	 * 
	 * The index checking/correction is done so multiplicites do not need to be specified for cmd/cmdReg or com/cmdResponse (with port names of seqCmdStatus and seqCmdBuff) blocks. 
	 * The latter pair has the additinal port name constraint because there are multiple other ports with the same cmdResponse type. Some of these port are not to be auto-indexed. 
	 * This makes it easier for the user to add and remove these ports without needing to worry about if the indexes of the pairings match.
	 * 
	 * <p> 
	 * 
	 * The program redefines the indexes in numerical order. Even if initial indexes are given, for the origin of Cmd and the target of CmdReg, they are overwritten. They target index of Cmd is preserved and also is written to the source of CmdReg. 
	 * 
	 * <p>
	 * METHODOLOGY
	 * <ul>
	 * 		<li>
	 * 			Iterate through all items of the physicalConnectionType and add those who have types of Cmd/CmdReg or those who have type of CmdResponse/Com and names of seqCmdStatus/seqCmdBuff to a list in a map where the index is the instance name of the associated cmdDispatch.
	 * 		</li>
	 *  	<li>
	 * 			Iterate through all keys in the dictionary.
	 *		</li>
	 * 			<ul>
	 * 				<li>
	 * 					Iterate through each item in the list and find another item that has the opposite target and source components but is off the same pair.
	 * 				</li>
	 * 				<li>
	 * 					If the associated item does not exist and if the search object is of the type of Cmd or Cmd reg, throw an error.
	 * 				</li>
	 * 				<li>
	 * 					If the item is of the type Com/CmdResponse or there is an associated pair, add the item(s) to the output list.
	 * 				</li>
	 * 			</ul>
	 * 		<li>
	 * 			Return a new list of physicalConnectionType objects with auto indexed IDS.
	 * 		</li>
	 * <ul>
	 * 
	 * 
	 * 
	 * @param pctSet Set of Physical Connection Types
	 * @return List of Physical Connection Types
	 */
	private static ArrayList<ISFSubsystem.physicalConnectionType> indexChecking(HashSet<ISFSubsystem.physicalConnectionType> pctSet) throws ConnectorException{
		ArrayList<ISFSubsystem.physicalConnectionType> pctList = new ArrayList<ISFSubsystem.physicalConnectionType>();
		
		HashMap<String , ArrayList<ISFSubsystem.physicalConnectionType>> indexCorrectionMap = new HashMap<String, ArrayList<ISFSubsystem.physicalConnectionType>>(); //Two ArrayLists, the first index is for Cmd, the second is for CmdReg
		
		
		for(ISFSubsystem.physicalConnectionType pct : pctSet){
			String componentName = null;

			if(pct.sourcePortType.equals("Cmd")){
				componentName = pct.source;
			}
			else if(pct.targetPortType.equals("CmdReg")){
				componentName = pct.target;
			}

			if(componentName != null){
				if(!indexCorrectionMap.containsKey(componentName)){
					indexCorrectionMap.put(componentName, new ArrayList<ISFSubsystem.physicalConnectionType>());
				}
				indexCorrectionMap.get(componentName).add(pct);
			}
			else{
				pctList.add(pct);
			}
		}
		for(String compName : indexCorrectionMap.keySet()){
			int cmdRegOrderingIndex = 0;
			int cmdSeqOrderingIndex = 0;
			while(indexCorrectionMap.get(compName).size() > 0){
				//System.out.println(indexCorrectionMap.get(compName));
				ISFSubsystem.physicalConnectionType searchPCT = indexCorrectionMap.get(compName).get(0);
				ISFSubsystem.physicalConnectionType currPCT = searchPCT;
				boolean noCompanionConn = true;
				int removeIndex = 0;
				for(int i = 1; i != indexCorrectionMap.get(compName).size(); i++){
					currPCT = indexCorrectionMap.get(compName).get(i);
					//Different Types
					if(
					       (searchPCT.sourcePortType.equals("Cmd") && currPCT.targetPortType.equals("CmdReg")) 
					       || 
					       (searchPCT.sourcePortType.equals("CmdReg") && currPCT.targetPortType.equals("Cmd"))
					   ) {
						//Check if they are of the same component
						//System.out.println("Search PCT (source: " + searchPCT.source + "  , target: " + searchPCT.target + ") " + "Curr PCT (target: " + currPCT.target + "  , source: " + currPCT.source + ")");
						if(searchPCT.source.equals(currPCT.target) && searchPCT.target.equals(currPCT.source)){
							if(searchPCT.sourcePortType.equals("CmdReg")){
								currPCT.target_index  = searchPCT.source_index;
								currPCT.source_index = cmdRegOrderingIndex;
								searchPCT.target_index = cmdRegOrderingIndex;
								cmdRegOrderingIndex++;
							}
							else if (searchPCT.sourcePortType.equals("Cmd")){
								searchPCT.target_index  = currPCT.source_index;
								searchPCT.source_index = cmdRegOrderingIndex;
								currPCT.target_index = cmdRegOrderingIndex;
								cmdRegOrderingIndex++;
							}
							noCompanionConn = false;
							removeIndex = i;
							break;
						}
					}
				}
				
				if(noCompanionConn){
					Utils.throwConnectorException("The connection from " + searchPCT.source + " in subsystem " + searchPCT.sourceRoleParentName + " to " + searchPCT.target + " in subsystem " + searchPCT.targetRoleParentName + " of type " + searchPCT.sourcePortType + " does not have an connection going from the same objects of the opposite type. Auto-assigning indexes cannot be continued.");
				}
				else{
					pctList.add(searchPCT);
					pctList.add(currPCT);
					indexCorrectionMap.get(compName).remove(removeIndex);
					indexCorrectionMap.get(compName).remove(0);
				}
			}
		}
		
		return pctList;
	}
	
	/**
	 * PHASE THREE FUNCTION
	 * <p>
	 * Checks if ports on leaf modules have any connections. If they do not, a warning is brought up onto the console.
	 * 
	 * <p>
	 * 
	 * This function first generates a Map with a generic leaf module's name as a key to a list of the "qualified name" of each port on the module. 
	 * The qualified name is used as an identifier for the generic module.
	 * 
	 * <p>
	 * 
	 * The function next iterates through all elements with the "PartProperty" stereotype to find "unique" leaf components. 
	 * The function now uses the unique leaf components generic name to iterate through each of its' ports. 
	 * 
	 * <p>
	 * 
	 * Each port is then checked to see if it exists as a key in the subsystemMap.
	 * If it doesn't exist, it means that this port has no connections and a warning flag is immediately notified. 
	 * 
	 * @throws ConnectorException
	 */
	private static void checkPorts() throws ConnectorException{
		HashMap<String , ArrayList<String>> moduleBaseNameMap = new HashMap<String , ArrayList<String>>();
		
		//Generates a Map with the base name of module is a key (IE SockGNDIF) associated with the "qualified name" of the module
		for(Element e : ISFSubsystem.getElementsOfStereotype("FlowPort")){
			Property prop = (Property) e;
			String moduleName = prop.getObjectParent().getHumanName();
			String qualified = prop.getQualifiedName();
			if(!e.getObjectParent().getHumanType().equals("Subsystem")){
				if(!moduleBaseNameMap.containsKey(moduleName)){
					moduleBaseNameMap.put(moduleName, new ArrayList<String>());
				}
				moduleBaseNameMap.get(moduleName).add(qualified);
			}
		}
		
		//Iterates through all modules
		for(Element e: ISFSubsystem.getElementsOfStereotype("PartProperty")){
			if(e != null){
				Property prop = (Property) e;
				if(prop.getType() != null){
					String baseModuleName = prop.getType().getHumanName();
					String qualified = prop.getQualifiedName();
					//Checks if subsystem or leaf module
					if(moduleBaseNameMap.containsKey(baseModuleName)){
						//Iterates through all ports on the generic module
						for(String portQualifiedName: moduleBaseNameMap.get(baseModuleName)){
							//Checks if the subsystem map contains the port
							if(!subsystemMap.containsKey(portQualifiedName + "_" + qualified)){
								Utils.notifyWarning("PortNotConnected - Port " + "<font color = 'blue'>" +portQualifiedName + "</font>" +" in component " + qualified + " has no connections.");
							}
							else if(qualified.equals("CDHCore::CDHSubsystem::sequenceRunner")){
								//System.out.println(portQualifiedName + "     " + moduleBaseNameMap.get(portQualifiedName + "_" + qualified).size());
							}
						}
					}
				}
				else{
					Utils.notifyWarning("NoElementType - Element " + e.getHumanName() + " with stereotype PartProperty does not have a type. Checking if ports on this element have connections has been skipped.");
				}
			}				
		}	
		
	}
	
	/**
	 * PHASE ONE FUNCTION
	 * <p>
	 * processModel has two main functions: to create a list of subsystems and to create a componentMap.
	 * 
	 * <p>
	 * 
	 * All the elements in the module are iterated through using the Element root. 
	 * 
	 * <p>
	 * 
	 * If the element type is Subsystem, the element is added to a list to be returned. 
	 * 
	 * <p>
	 * 
	 * If the element is an instance of a subsystem, the subsystemOwners map is updated, where each key (name of subsystem) is associated with an list of two string arrays.
	 * The first String corresponds to the instance name and the second corresponds to the name of the owner of the subsystem.
	 * 
	 * @param root Root of the system
	 * @return List of subsystem elements
	 */
	private static List<Element> processModel(Element root) throws ConnectorException{
		List<Element> subsystemElementList = new ArrayList<Element>();
		Collection<Element> coll = (Collection<Element>)ModelHelper.getElementsOfType(root, null, true, true);

		for(Element el : coll) {
			if(el instanceof NamedElement) {
				if (el.getHumanType().equals("Subsystem")) {
					subsystemElementList.add(el);
					subsystemNameSet.add(((NamedElement) el).getID());
				}
				else if (el.getHumanType().equals("Part Property") || el.getHumanType().equals("Reference Property") ){
					Property prop = (Property) el;
					if(prop.getType() != null){
						if(prop.getType().getHumanType().equals("Subsystem")){
							
							String subName = ISFSubsystem.getLastPartOfString(prop.getType().getHumanName());
							if(!subsystemOwners.containsKey(subName)){
								subsystemOwners.put(subName, new ArrayList<String[]>());
							}
							//[instanceName , genericName]
							String[] addString = {prop.getName() , ISFSubsystem.getLastPartOfString(prop.getOwner().getHumanName())};

							subsystemOwners.get(subName).add(addString);
						}
					}
					else{
						Utils.notifyWarning("NoElementType - Component " + prop.getName() + " in subsystem " + prop.getObjectParent().getHumanName() + " has no type object. Please check the subsystem to see if extraneous elements exist.");
					}
				}
			}
		}
		return(subsystemElementList);
	}

	/**
	 * Checks the subsystemInstances map (which is a key string mapped to a value of how many times this class is instantiated.
	 *  Will return true if it is a singular system, false if the system isn't instantiated or the instances equal one.
	 * @param key
	 * @return
	 */
	private static boolean isTopSubsystemNode(String key){
		if(!subsystemInstances.containsKey(key)){
			return false;
		}
		return(subsystemInstances.get(key) == 1);
	}
	
	/**
	 * PHASE ONE FUNCTION
	 * <p>
	 * processSubsystem populates both the subsystemMap as well as the sourcePorts/targetPorts list.
	 * 
	 * <p>
	 * 
	 * First, the function's name is used with the subsystemOwners map to create a list tree. This list tree is then used to generate all the different names needed to clarify 
	 * 
	 * <p>
	 * 
	 * 
	 * The function iterates through all Connectors associated with the subsystem. The subsystemMap, sourcePorts, and targetPorts are populated through this iteration.
	 * 
	 * <p>
	 * 
	 * The subsystemMap is a Map where the key is the ID of each port with an associated list of input and output connectors.
	 * 
	 * <p>
	 * 
	 * ConnectorEnds of leaf components are also pushed onto sourcePorts and targetPorts lists, respective of what direction they accept data. 
	 * 
	 * 
	 * <p>
	 * 
	 * Using the list of all instances, each internal module (bbd) is added as an ISFSubsystem componentType to the component map, with the new name of the module
	 * 
	 * @param subsystem Subsystem Element
	 * @throws ConnectorException
	 */
	private static void processSubsystem(Element subsystem)
			throws ConnectorException {
		/*
		 * Map creation
		 * Creates map based off qualified port name
		 * map[qualified name] = Connectors[]
		 */
		Utils.printToAll("*** Subsystem = " + subsystem.getHumanName());
		//Go through all parents of the subsystem to see if the components within the subsystem need special names
		String subsystemName = ISFSubsystem.getLastPartOfString(subsystem.getHumanName());
		ArrayList<String[]> subsystemWithinSubsystemList = new ArrayList<String[]>();
		ArrayList<String> subsystemInstanceNameList = new ArrayList<String>();
		String currSubsystemName = subsystemName;
		String currInstanceName = "";
		int index = 0;
		//Infinite loop protection
		while(index < 50){
			if(subsystemOwners.containsKey(currSubsystemName)){
				for(String[] e: subsystemOwners.get(currSubsystemName)){
					String instanceName = e[0];
					if(!currInstanceName.equals("")){
						instanceName += "_" + currInstanceName;
					}

					String[] tuple = {instanceName, e[1]};
					subsystemWithinSubsystemList.add(index , tuple);
				}
			}
			else{
				subsystemInstanceNameList.add(currInstanceName);
				String prefix = "";
				ArrayList<String> reveredPre = new ArrayList<String>();
				for(String pre : currInstanceName.split("_")){
					if(!pre.equals("")){
						reveredPre.add(0 , pre);
					}
				}
				for(String pre : reveredPre){
					if(!prefix.equals("") && !pre.equals("")){
						prefix = pre + "_" + prefix;
					}
					
					
					if(!uniqueSubsystemInstanceNames.containsKey(prefix)){
						uniqueSubsystemInstanceNames.put(prefix, new ArrayList<String>());
					}
					String instanceName = currInstanceName.replace(prefix, "");
					uniqueSubsystemInstanceNames.get(prefix).add(instanceName);
				}
			}
			if(index >= subsystemWithinSubsystemList.size()){
				break;
			}
			
			currSubsystemName = subsystemWithinSubsystemList.get(index)[1];
			currInstanceName = subsystemWithinSubsystemList.get(index)[0];
			index++;
		}
		
		
		subsystemInstances.put(subsystemName, subsystemInstanceNameList.size());
		
		//Iterate through all owned objects of the inputed subsystem
		for(Element e: subsystem.getOwnedElement()){
			//Add connections to a map where the connector end is a key associated with lists of input/output connections
			if(e.getHumanType().equals("Part Property") || e.getHumanType().equals("Reference Property")){
				Property prop = (Property) e;
				if(prop.getType() != null){
					if(!prop.getType().getHumanType().equals("Subsystem")){
						if(prop.get_connectorEndOfPartWithPort().toArray().length > 0){
							if(subsystemInstanceNameList.size() == 0){
								subsystemInstanceNameList.add("");
							}
							for(String instanceName : subsystemInstanceNameList){
								if(isTopSubsystemNode(subsystemName)){
									instanceName = "";
								}
								String id = instanceName+prop.getName();
								String name = prop.getName();
								if(!instanceName.equals("")){
									name = instanceName+"_"+ name;
								}
								
								String[] typeList = ((ConnectorEnd) prop.get_connectorEndOfPartWithPort().toArray()[0]).getRole().getObjectParent().getHumanName().split(" ");
								String type = typeList[typeList.length - 1];
								Element baseComponent = (Element) ((ConnectorEnd) prop.get_connectorEndOfPartWithPort().toArray()[0]).getRole().getObjectParent();
								String nameSpace = ISFSubsystem.getNamespace(baseComponent);
								
								String baseID = ISFSubsystem.getBaseId((Element) prop);
								if(baseID.equals("")){
									baseID = ISFSubsystem.getBaseId(baseComponent);
								}
								
								String instanceWindow = ISFSubsystem.getInstanceWindow((Element) prop);
								if(instanceWindow.equals("")){
									instanceWindow = ISFSubsystem.getInstanceWindow(baseComponent);
								}

								String XMLPath = ISFSubsystem.getFileLocation(baseComponent);
								componentMap.put(id, ISFSubsystem.createComponent(name, nameSpace, type, baseID, instanceWindow , XMLPath));
							}
							
						}
					}
				}
			}
			else if (e instanceof Connector) {
				Connector tempConnector = (Connector) e;
				String sourceID = ISFSubsystem.getQualifiedSourcePort(tempConnector);
				String targetID = ISFSubsystem.getQualifiedTargetPort(tempConnector);
				if(!subsystemMap.containsKey(sourceID)){
					subsystemMap.put(sourceID , new ArrayList<ArrayList<Connector>>());
					ArrayList<Connector> targetList = new ArrayList<Connector>();
					ArrayList<Connector> sourceList = new ArrayList<Connector>();
					subsystemMap.get(sourceID).add(sourceList);
					subsystemMap.get(sourceID).add(targetList);
				}
				if(!subsystemMap.containsKey(targetID)){
					subsystemMap.put(targetID , new ArrayList<ArrayList<Connector>>());
					ArrayList<Connector> targetList = new ArrayList<Connector>();
					ArrayList<Connector> sourceList = new ArrayList<Connector>();
					subsystemMap.get(targetID).add(sourceList);
					subsystemMap.get(targetID).add(targetList);
				}
				subsystemMap.get(sourceID).get(0).add(tempConnector); //0 is source (output)
				subsystemMap.get(targetID).get(1).add(tempConnector); //1 is target (input)
				

				//Add connector ends to their respective sourcePort or targetPort lists if found to be on a leaf component
				for(int i = 0; i!= 2; i++){
					ConnectorEnd tempPort = ISFSubsystem.getConnEnd(tempConnector, i==0);
					if(!tempPort.getRole().getObjectParent().getHumanType().equals("Subsystem")){
						if(tempPort.getRole().getType() == null){
							Utils.throwConnectorException("Port " + tempPort.getRole().getName() + " on Component " + tempPort.getPartWithPort().getObjectParent().getHumanName() + " has no data type.");
						}
						
						if(i==0){
							sourcePorts.add(tempPort);
						}
						else{
							targetPorts.add(tempPort);
						}
					}
				}
				
			}
		}
	}
	
	/**
	 * PHASE TWO FUNCTION 
	 * <p>
	 * Iterates through the list of source ports and tries to find the final destination, while checking for errors. 
	 *
	 * <p>
	 * 
	 * For each source port, the program traces down it's path by accessing the subsystemMap with the port's ID. 
	 * All connections associated with the port are put onto a list, the multiplicity is reevaluated, and the program moves onto processing the next port on the list.
	 * Every time a new connector is accessed, the target is checked to see if is a leaf target with the same type as the source.
	 * If it is, an ISFSubsystem physicallyConnectionType object is put onto a list. 
	 * 
	 * <p>
	 * 
	 * Additionally, to support multiple instances of ibds, is a a source instance and target instance name is created along side every connection. 
	 * This name is the the root modules name followed by each subsystem which the connection's path goes through. 
	 * This name is used if the program detect that target or source subsystem is used more than once.
	 * 
	 * <p>
	 * 
	 * Exceptions are thrown in the instances where
	 * <ul> 
	 * 		<li>Source multiplicity is repeated by the same port onto multiple connectors.</li>
	 * 		<li>A source on a singular branch of travel (no other sources share the same path) has a target port at the end with a different type as the source port.</li>
	 * </ul>
	 * 
	 * 
	 * @throws ConnectorException 
	 * 
	 */
	private static void createConnections() throws ConnectorException{
		/*
		 * for source in sourcePorts:
		 * 		for mIndex in multiplicityItems:
		 * 			for connectors in toTravelConnectors:
		 * 				if current connector is not a final port:
		 * 					add all associated connectors with the right multiplicities to toTravelConnectors
		 * 				else:
		 * 					add initial connector and final connector information to masterList
		 * 					break
		 * 		
		 */
		//Iterate through all source (output) ports
		System.out.println("Connector Creation start");
		for(ConnectorEnd currentConnectorEnd : sourcePorts){
			if(subsystemMap.containsKey(ISFSubsystem.getQualifiedPort(currentConnectorEnd) )){
				String qualfiedName = ISFSubsystem.getQualifiedPort(currentConnectorEnd); //Key for subsystemMap
				int originalMultiplicity = currentConnectorEnd.getLower();
				int startIndex = 0;
				int endIndex = originalMultiplicity;
				
				if(currentConnectorEnd.getLowerValue() != null && currentConnectorEnd.getUpperValue() != null){
					startIndex = currentConnectorEnd.getLower();
					endIndex = currentConnectorEnd.getUpper() + 1;
				}
				else if(currentConnectorEnd.getLowerValue() != null){
					startIndex = currentConnectorEnd.getLower();
					endIndex = startIndex + 1;
				}
				
				//multiplicityIndex for each multiplicity values
				for(int multiplicityIndex = startIndex; multiplicityIndex != endIndex; multiplicityIndex++){	
					//Utils.printToAll("On Multiplicity: " + multiplicityIndex);
					int i = 0;
					int currentMultiplicity = multiplicityIndex;
					String currentSourceName = "";
					String currentTargetName = "";
					String prevSourceName = "";
					qualfiedName = ISFSubsystem.getQualifiedPort(currentConnectorEnd);
					
					boolean hasMultipleBackwardsSourceConnections = false; //Tracks if when checking the out connections at a certain node if there is more than one (To detect common service like branches) 
					boolean oneConnectionFound = false; //Checks if at least one connection was created, used to avoid the single branch connector exception
					
					ConnectorEnd targetConnEnd  = currentConnectorEnd;
					ConnectableElement targetRole = currentConnectorEnd.getRole();
					Property targetPart = currentConnectorEnd.getPartWithPort();
					
					ArrayList<Connector> connList = new ArrayList<Connector>(); //connectors in order of path
					ArrayList<Integer> multList = new ArrayList<Integer>(); //Indexes of multiplicites are associated with the conn list
					ArrayList<String> sourceNameList = new ArrayList<String>();
					ArrayList<String> targetNameList = new ArrayList<String>();
					
					
					//Iterate through all the connectors associated with the the currPort
					while(i < 101){ //Infinite loop prevention.
						String previousConnectorTargetID = ""; //Used to figure out if current connector target and previous connector target are the same
						int previousConnectorTargetMult = 0;
						HashSet<Integer>  portMultSet = new HashSet<Integer>();
						
						//Adds connectors (from subsystem map) to be analyzed in the above while loop
						for( Connector e : subsystemMap.get(qualfiedName).get(0)){
							
							ConnectorEnd tempSourceConnector = ISFSubsystem.getSourceConnEnd(e);
							ConnectorEnd tempTargetConnector = ISFSubsystem.getTargetConnEnd(e);
							
							//Utils.printToAll(qualfiedName+ " : (Lower:" + tempSourceConnector.getLower()+ " , Upper:"+tempSourceConnector.getUpper()+")");
							
							if(tempSourceConnector.getLowerValue() != null && portMultSet.contains(tempSourceConnector.getLower())){ //Find connector with source that has the same multiplicity that we are looking for
								Utils.throwConnectorException("Connector " + e.getName() + " in " + e.getObjectParent().getHumanName() + " with ends "+
										tempSourceConnector.getRole().getQualifiedName()+" and "+tempTargetConnector.getRole().getQualifiedName()+
										" has the same multiplicity (source "+tempSourceConnector.getLower()+") as another connector with the same source port.");
							}
							portMultSet.add(tempSourceConnector.getLower());
							
							if(previousConnectorTargetID.equals(ISFSubsystem.getQualifiedPort(tempTargetConnector))){ //Check the current connector's target end is equal to the previous connector's target end
								previousConnectorTargetMult ++;
							}
							else{
								previousConnectorTargetMult = 0;
							}
							previousConnectorTargetID = ISFSubsystem.getQualifiedPort(tempTargetConnector);
								
							/*
							 * if ( the current multiplicity is between the lower and upper multiplicities of the source connector AND the source multiplicity is not null OR
							 *	the source multiplicity is null and the target multiplicity is null OR
							 *	the source multiplicity is null and the current multiplicity is equal to a counter which keeps track of how long we we have had separate connectors iterate on the same  OR 
							 *	the connectors associated with this port is one and the source multiplicity is null)
							 */

							if(((tempSourceConnector.getLower() <= currentMultiplicity && tempSourceConnector.getUpper() >=  currentMultiplicity) && tempSourceConnector.getLowerValue() != null) 
									|| (tempSourceConnector.getLowerValue() == null && tempTargetConnector.getLowerValue() == null)
									|| (tempSourceConnector.getLowerValue() == null && previousConnectorTargetMult == currentMultiplicity) 
									|| (subsystemMap.get(qualfiedName).get(0).toArray().length == 1 && tempSourceConnector.getLowerValue() == null)){
								
								connList.add(i , e);
								multList.add(i , generateNewMultiplicity(e , currentMultiplicity , previousConnectorTargetMult));
								
								//Collect source name is partWithPort exists
								if(tempSourceConnector.getPartWithPort() != null){
									if(currentSourceName == ""){
										sourceNameList.add(i , tempSourceConnector.getPartWithPort().getName());
									}
									else{
										sourceNameList.add(i , tempSourceConnector.getPartWithPort().getName() +"_"+currentSourceName);
									}
								}
								else{
									sourceNameList.add(i , currentSourceName);
								}
								//Collect target name if partWithPort exists
								if(tempTargetConnector.getPartWithPort() != null){
									if(currentTargetName == ""){
										targetNameList.add(i , tempTargetConnector.getPartWithPort().getName());
									}
									else{
										targetNameList.add(i , currentTargetName+"_"+tempTargetConnector.getPartWithPort().getName());
									}
									
								}
								else{
									targetNameList.add(i , currentTargetName);
								}
								
								//targetNameList.add(i , e.getName());
							}
							
						}
						//Check if a source port has multiple backwards connections
						if(subsystemMap.get(qualfiedName).get(1).size() > 1){
							hasMultipleBackwardsSourceConnections = true;
						}
						
						//No more connections to explore. Throw an error or exit quietly
						if(connList.size() <= i){
							String errorString = "Source Port " + currentConnectorEnd.getRole().getHumanName() + " on Subsystem " + currentConnectorEnd.getPartWithPort().getObjectParent().getHumanName();
							if(connList.size() == 0){ //Port is not connected to anything
								Utils.notifyWarning("PortNotConnected - " + errorString + " has no outgoing connections.");
							}
							if(!hasMultipleBackwardsSourceConnections && !targetRole.getObjectParent().getHumanType().equals("Subsystem") && !oneConnectionFound){
								Utils.throwConnectorException(errorString + " is on a singular branch and is connected to the leaf port " + targetRole.getName() + " on component " + targetPart.getObjectParent().getHumanName() + ". The connection may have different data types. (Start Multiplicity: "+multiplicityIndex+ " , Current Multiplicity: " +currentMultiplicity+ ")");
							}
							
							//System.out.println("OUT OF SEARCHABLE CONNECTIONS");
							break;
						}
						
						targetConnEnd = ISFSubsystem.getTargetConnEnd(connList.get(i));
						targetRole = targetConnEnd.getRole();
						targetPart = targetConnEnd.getPartWithPort();
						qualfiedName = ISFSubsystem.getQualifiedTargetPort(connList.get(i));
						currentMultiplicity = multList.get(i);
						prevSourceName = currentSourceName;
						currentSourceName = sourceNameList.get(i);
						currentTargetName = targetNameList.get(i);
						
						/*
						 * If(start and current qualified names aren't equal AND the current port Type isn't null) AND
						 * 		target human type is subsystem and the start type and end type are equal AND
						 * 		the connector end is a target port
						 */
						if(!targetRole.getQualifiedName().equals(currentConnectorEnd.getRole().getQualifiedName()) && ISFSubsystem.getTargetConnEnd(connList.get(i)).getRole().getType() != null){
							if(!targetRole.getObjectParent().getHumanType().equals("Subsystem") && ISFSubsystem.getTargetConnEnd(connList.get(i)).getRole().getType().getName().equals(currentConnectorEnd.getRole().getType().getName())){
								if(((EnumerationLiteral) StereotypesHelper.getStereotypePropertyFirst(targetRole, StereotypesHelper.getFirstVisibleStereotype(targetRole) , "direction")).getName().equals("in")){
									//Crease ISFSubsystem Object
									int source_index = multiplicityIndex;
									int target_index = currentMultiplicity;
									
									boolean sourceSubsystemIsSingular = false;
									//If there are multiple subsystem instances of the source port
									if (isTopSubsystemNode(ISFSubsystem.getLastPartOfString(currentConnectorEnd.getPartWithPort().getObjectParent().getHumanName()))){
										currentSourceName = currentConnectorEnd.getPartWithPort().getName();
										sourceSubsystemIsSingular = true;
									}
									//If there are multiple subsystem instances of the target port
									if (isTopSubsystemNode(ISFSubsystem.getLastPartOfString(targetConnEnd.getPartWithPort().getObjectParent().getHumanName()))){
										currentTargetName = targetPart.getName();
									}
									prevSourceName = prevSourceName.replace(currentConnectorEnd.getPartWithPort().getName(), "");
									//Create multiple connections if the connections happen in a subsystem that is instantiated multiple times.
									if(!sourceSubsystemIsSingular && uniqueSubsystemInstanceNames.containsKey(prevSourceName)){
										for(String instancePrefix : uniqueSubsystemInstanceNames.get(prevSourceName)){
											ISFSubsystem.physicalConnectionType tempPC = new ISFSubsystem.physicalConnectionType(currentConnectorEnd, targetConnEnd, instancePrefix+"_"+currentSourceName , instancePrefix+"_"+currentTargetName , source_index, target_index);
											physicalConnectionSet.add(tempPC);
										}
									}
									else{
										ISFSubsystem.physicalConnectionType tempPC = new ISFSubsystem.physicalConnectionType(currentConnectorEnd, targetConnEnd, currentSourceName , currentTargetName , source_index, target_index);
										
										physicalConnectionSet.add(tempPC);
										
									}
									oneConnectionFound = true;						
								}
							}
						}
						
						i++;
					}
					if(i > 99){
						Utils.printToAll("TIMEOUT");
					}
				}
			}
			
			
		}
	}
	
	/**
	 * PHASE TWO FUNCTION
	 * <p>
	 * generateMultiplicity is used within createConnections to find the multiplicity of the next connector. 
	 * 
	 * <p>
	 * 
	 * The function takes in three arguments: the current Connector, the multiplicity of the source of the connector (calculated from this function in the previous iteration of the loop) and 
	 * a 'prevousConnectorTargetMult', which is an integer which indicates the 'inorder' position of the target of the Connector by keeping a track of how many connectors have accessed the same end. 
	 * 
	 * <p>
	 * 
	 * 
	 * @param e Connector element
	 * @param currentMultiplicity The current multiplicity value of the connector
	 * @param previousConnectorTargetMult Multiplicity value based on the position of the connector in the port.
	 * 
	 * @return New multiplicity value to be used in next iteration
	 * @throws ConnectorException Only ISFSubsystem.getConnEnd() will raise the exception in this function.
	 */
	public static int generateNewMultiplicity(Connector e, int currentMultiplicity , int previousConnectorTargetMult) throws ConnectorException{
		ConnectorEnd tempSourceConnector = ISFSubsystem.getSourceConnEnd(e);
		ConnectorEnd tempTargetConnector = ISFSubsystem.getTargetConnEnd(e);
		int tempMult = 0;
		
		//Utils.printToAll("Source Mult Name: " + tempSourceConnector.getRole().getHumanName() + "(L:"+tempSourceConnector.getLower()+" , U:"+tempSourceConnector.getUpper()+" ) Target Mult Name: " + tempTargetConnector.getRole().getHumanName() + "(L:"+tempTargetConnector.getLower()+" , U:"+tempTargetConnector.getUpper()+" )");

		//Checks to see if upper and lower multiplicity are the same. If they aren't propagate forward with (current multiplicity - the source size) + the target size. Many to many.
		if(tempSourceConnector.getLower() != tempSourceConnector.getUpper()){
			tempMult = (currentMultiplicity - ISFSubsystem.getSourceIndex(e)) + ISFSubsystem.getTargetIndex(e);
		}
		
		//If the source port and target port have above 1 multiplicities, have tempMult become the difference between the two + or - (depending on the flow) of the current mult. One to many.
		else if(((Port) tempSourceConnector.getRole()).getLowerValue() != null && ((Port) tempTargetConnector.getRole()).getLowerValue() != null){
			int sourcePortLower = ((Port) tempSourceConnector.getRole()).getLower();
			int targetPortLower = ((Port) tempTargetConnector.getRole()).getLower();
			if(sourcePortLower < targetPortLower){
				tempMult = currentMultiplicity + (targetPortLower - sourcePortLower);
			}
			else{
				tempMult = currentMultiplicity - (sourcePortLower - targetPortLower);
			}
		}

		//Checks if target multiplicity is specified
		else if(tempTargetConnector.getLowerValue() != null && tempTargetConnector.getLower() == tempTargetConnector.getUpper()){
			tempMult = ISFSubsystem.getTargetIndex(e);
		}
		else if(tempTargetConnector.getLowerValue() != null && tempTargetConnector.getLower() < tempTargetConnector.getUpper()){
			tempMult = tempTargetConnector.getLower() + previousConnectorTargetMult;
		}
		//Carry old mult forward
		else{ //one to many
			tempMult = previousConnectorTargetMult;
		}
		
		return tempMult;
	}
	
	
}
