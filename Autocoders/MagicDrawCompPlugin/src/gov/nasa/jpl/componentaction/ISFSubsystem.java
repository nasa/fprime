package gov.nasa.jpl.componentaction;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import com.nomagic.magicdraw.core.Application;
//import com.nomagic.magicdraw.core.Application;
import com.nomagic.uml2.ext.jmi.helpers.StereotypesHelper;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Element;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.EnumerationLiteral;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Type;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.MultiplicityElement;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Property;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Class;
import com.nomagic.uml2.ext.magicdraw.components.mdbasiccomponents.Component;
import com.nomagic.uml2.ext.magicdraw.compositestructures.mdinternalstructures.ConnectableElement;
import com.nomagic.uml2.ext.magicdraw.compositestructures.mdinternalstructures.Connector;
import com.nomagic.uml2.ext.magicdraw.compositestructures.mdinternalstructures.ConnectorEnd;
import com.nomagic.uml2.ext.magicdraw.compositestructures.mdports.Port;
import com.nomagic.uml2.ext.magicdraw.mdprofiles.Stereotype;

/**
 * Helper class for ProcessISFTopology 
 *
 */
public class ISFSubsystem {

	/**
	 * Used as a object to store the componentMap, physicalConnectionList, name, baseID, and instanceWindow. An object of this type is then sent to IsfSubXmlWriter and is used to write to 
	 * the XML output file. 
	 *
	 */
	public static class topologyModel{
		HashMap<String , ISFSubsystem.componentType> componentMap;
		ArrayList<ISFSubsystem.physicalConnectionType> physicalConnectionList;
		String baseID;
		String instanceWindow;
		String name;
		
		public topologyModel(HashMap<String , ISFSubsystem.componentType> componentItemsMap  , ArrayList<ISFSubsystem.physicalConnectionType> physicalConnList){
			componentMap = componentItemsMap;
			physicalConnectionList = physicalConnList;
		}
		
		public HashMap<String , ISFSubsystem.componentType>  getComponentMap(){
			return componentMap;
		}
		
		public ArrayList<ISFSubsystem.physicalConnectionType>  getPhysicalConnectionList(){
			return physicalConnectionList;
		}
		
		public String getBaseID(){
			return baseID;
		}
		
		public String getInstanceWindow(){
			return instanceWindow;
		}
		
		public String getName(){
			return name;
		}
		
	}

	/**
	 * Used as a dataObjet within the componentMap. The objects within this object are used to describe a leaf component. The methods are pretty self-explanatory. 
	 *
	 */
	public static class componentType {
		String name;
		String nameSpace;
		String type;
		String baseID;
		String instanceWindow;
		String XMLLocation;
		
		public componentType(){
			
		}
		
		public String getName(){
			return name;
		}
		
		public String getNameSpace(){
			return nameSpace;
		}
		
		public String getType(){
			return type;
		}
		
		public String getBaseID(){
			return baseID;
		}
		
		public String getInstanceWindow(){
			return instanceWindow;
		}
		
		public String getXMLLocation(){
			return XMLLocation;
		}
		
	}
	

	/**
	 * The physicalConnectionType is used to describe connections. Connector objects can not be re-defined through the API, so when we find a source port with it's final target port, we make one 
	 * of these objects to describe the connection. This includes the names of the ports, the type, the parent name, the multiplicities, and the actual connectorEnd.
	 */
	public static class physicalConnectionType {
		String source;
		String target;
		String sourcePortName;
		String targetPortName;
		String sourcePortType;
		String targetPortType;
		String targetRoleParentName;
		String sourceRoleParentName;
		ConnectorEnd sourceEnd;
		ConnectorEnd targetEnd;
		
		Integer source_index;
		Integer target_index;

		public physicalConnectionType(ConnectorEnd sourceEnd , ConnectorEnd targetEnd , Integer source_index , Integer target_index) {
			this.sourceEnd = sourceEnd;
			this.targetEnd = targetEnd;
			
			Property sourcePart = sourceEnd.getPartWithPort();
			Property targetPart = targetEnd.getPartWithPort();
			
			ConnectableElement sourceRole = sourceEnd.getRole();
			ConnectableElement targetRole = targetEnd.getRole();

				
			this.source = sourcePart.getName();
			this.target = targetPart.getName();
			this.sourcePortName = sourceRole.getName();
			this.targetPortName = targetRole.getName();
			this.sourcePortType = sourceRole.getType().getName();
			this.targetPortType = targetRole.getType().getName();
			this.source_index = source_index;
			this.target_index = target_index;
			this.targetRoleParentName = targetPart.getObjectParent().getHumanName();
			this.sourceRoleParentName = sourcePart.getObjectParent().getHumanName();
		}
		
		
		public physicalConnectionType(ConnectorEnd sourceEnd , ConnectorEnd targetEnd , String sourceName , String targetName , Integer source_index , Integer target_index) {
			this.sourceEnd = sourceEnd;
			this.targetEnd = targetEnd;
			
			ConnectableElement sourceRole = sourceEnd.getRole();
			ConnectableElement targetRole = targetEnd.getRole();

			Property sourcePart = sourceEnd.getPartWithPort();
			Property targetPart = targetEnd.getPartWithPort();	
			
			this.source = sourceName;
			this.target = targetName;
			this.sourcePortName = sourceRole.getName();
			this.targetPortName = targetRole.getName();
			this.sourcePortType = sourceRole.getType().getName();
			this.targetPortType = targetRole.getType().getName();
			this.source_index = source_index;
			this.target_index = target_index;
			this.targetRoleParentName = targetPart.getObjectParent().getHumanName();
			this.sourceRoleParentName = sourcePart.getObjectParent().getHumanName();
		}
		
		
		
		@Override
		public String toString() {
			return "physicalConnectionType [source=" + source + ", target=" + target + ", sourcePortName="
					+ sourcePortName + ", targetPortName=" + targetPortName + ", sourcePortType=" + sourcePortType
					+ ", targetPortType=" + targetPortType + ", targetRoleParentName=" + targetRoleParentName
					+ ", sourceRoleParentName=" + sourceRoleParentName + ", source_index=" + source_index + ", target_index=" + target_index + "]";
		}


		@Override
		public int hashCode() {
			final int prime = 31;
			int result = 1;
			result = prime * result + ((source == null) ? 0 : source.hashCode());
			result = prime * result + ((sourcePortName == null) ? 0 : sourcePortName.hashCode());
			result = prime * result + ((sourcePortType == null) ? 0 : sourcePortType.hashCode());
			result = prime * result + ((sourceRoleParentName == null) ? 0 : sourceRoleParentName.hashCode());
			result = prime * result + ((source_index == null) ? 0 : source_index.hashCode());
			result = prime * result + ((target == null) ? 0 : target.hashCode());
			result = prime * result + ((targetPortName == null) ? 0 : targetPortName.hashCode());
			result = prime * result + ((targetPortType == null) ? 0 : targetPortType.hashCode());
			result = prime * result + ((targetRoleParentName == null) ? 0 : targetRoleParentName.hashCode());
			result = prime * result + ((target_index == null) ? 0 : target_index.hashCode());
			
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
			physicalConnectionType other = (physicalConnectionType) obj;
			if (source == null) {
				if (other.source != null)
					return false;
			} else if (!source.equals(other.source))
				return false;
			if (sourcePortName == null) {
				if (other.sourcePortName != null)
					return false;
			} else if (!sourcePortName.equals(other.sourcePortName))
				return false;
			if (sourcePortType == null) {
				if (other.sourcePortType != null)
					return false;
			} else if (!sourcePortType.equals(other.sourcePortType))
				return false;
			if (sourceRoleParentName == null) {
				if (other.sourceRoleParentName != null)
					return false;
			} else if (!sourceRoleParentName.equals(other.sourceRoleParentName))
				return false;
			if (source_index == null) {
				if (other.source_index != null)
					return false;
			} else if (!source_index.equals(other.source_index))
				return false;
			if (target == null) {
				if (other.target != null)
					return false;
			} else if (!target.equals(other.target))
				return false;
			if (targetPortName == null) {
				if (other.targetPortName != null)
					return false;
			} else if (!targetPortName.equals(other.targetPortName))
				return false;
			if (targetPortType == null) {
				if (other.targetPortType != null)
					return false;
			} else if (!targetPortType.equals(other.targetPortType))
				return false;
			if (targetRoleParentName == null) {
				if (other.targetRoleParentName != null)
					return false;
			} else if (!targetRoleParentName.equals(other.targetRoleParentName))
				return false;
			if (target_index == null) {
				if (other.target_index != null)
					return false;
			} else if (!target_index.equals(other.target_index))
				return false;
			return true;
		}


		public int getSourcePortIndex(){
			return source_index;
		}
		
		public int getTargetPortIndex(){
			return target_index;
		}

		public String getSource() {
			return source;
		}

		public String getTarget() {
			return target;
		}

		public String getSourcePortName() {
			return sourcePortName;
		}

		public String getTargetPortName() {
			return targetPortName;
		}

		public String getSourcePortType() {
			return sourcePortType;
		}

		public String getTargetPortType() {
			return targetPortType;
		}
	}

	

	/**
	 * Creates and returns new componentType
	 * 
	 * 
	 * @param name Component name
	 * @param nameSpace Component name space
	 * @param type Component Type
	 * @param baseID Base ID
	 * @param instanceWindow Instance Window ID
	 * @return componentType
	 */
	public static componentType createComponent(String name , String nameSpace , String type, String baseID , String instanceWindow , String xmlPath){
		componentType out = new componentType();
		out.name = name;
		out.nameSpace = nameSpace;
		out.type = type;
		out.baseID = baseID;
		out.instanceWindow = instanceWindow;
		out.XMLLocation = xmlPath;
		return(out);
	}
	
	
	/**
	 * Returns the multiplicity of the source end of the connector.
	 * 
	 * @param conn Connector
	 * @return multiplicity 
	 * @throws ConnectorException
	 */
	public static int getSourceIndex(Connector conn) throws ConnectorException {
		if (getSourceConnEnd(conn).getLowerValue() == null) {
			return 0;
		}
		return ((MultiplicityElement) getSourceConnEnd(conn)).getLower();
	}

	/**
	 * Returns the multiplicity of the target end of the connector.
	 * 
	 * @param conn Connector
	 * @return multiplicity 
	 * @throws ConnectorException
	 */
	public static int getTargetIndex(Connector conn) throws ConnectorException {
		if (getTargetConnEnd(conn).getLowerValue() == null) {
			return 0;
		}
		return ((MultiplicityElement) getTargetConnEnd(conn)).getLower();
	}
	



	public static void print() {
		System.out
				.println("--------------------------------------------------");
		System.out.println("---Topology Status:");
		System.out
				.println("--------------------------------------------------");
		// printComponents();
		// printPhysicalConnections();
		System.out
				.println("--------------------------------------------------");
	}

	
	public static String getFileLocation(Element instanceElement){
		Class ie = (Class) instanceElement;
		String url = "";
		try{
			url = ie.getOwningPackage().getURI() + "/"; // + ie.getName();
		}
		catch(Exception e){
			url = ie.getName();
		}
		
		return(url);
	}



	/**
	 * Returns the base ID of the element.
	 * 
	 * @param instanceElement
	 * @return
	 */
	public static String getBaseId(Element instanceElement){
		List<String> propvals = StereotypesHelper.getStereotypePropertyValueAsString(instanceElement,"ComponentInstance", "BaseID");
		if (propvals.size() > 0) {
			return propvals.get(0);
		} else {
			return "";
		}
	}
	
	/**
	 * Returns the instance window of the element.
	 * 
	 * @param instanceElement
	 * @return
	 */
	public static String getInstanceWindow(Element instanceElement){
		List<String> propvals = StereotypesHelper.getStereotypePropertyValueAsString(instanceElement,"ComponentInstance", "Window");
		if (propvals.size() > 0) {
			return propvals.get(0);
		} else {
			return "";
		}
	}

	/**
	 * Returns the source name off the attached component.
	 * 
	 * @param c Connector
	 * @return Component Name
	 * @throws ConnectorException Will raise an exception if the target component does not exist (IE the source is connected to the outside of the subsystem)
	 */
	public static String getSourceComp(Connector c) throws ConnectorException {
		if (getSourceConnEnd(c).getPartWithPort() == null) {
			throw new ConnectorException(getSourceConnEnd(c).getRole()
					.getQualifiedName() + " Connection has no source component");
		}
		return getCompName(getSourceConnEnd(c));
	}

	/**
	 * Returns the target name off the attached component.
	 * 
	 * @param c Connector
	 * @return Component Name
	 * @throws ConnectorException Will raise an exception if the target component does not exist (IE the target is connected to the outside of the subsystem)
	 */
	public static String getTargetComp(Connector c) throws ConnectorException {
		if (getTargetConnEnd(c).getPartWithPort() == null) {
			throw new ConnectorException(getTargetConnEnd(c).getRole()
					.getQualifiedName() + " Connection has no target component");
		}
		return getCompName(getTargetConnEnd(c));
	}
	
	/**
	 * Returns the component name of the connectorEnd.
	 * @param conn
	 * @return
	 * @throws ConnectorException
	 */
	public static String getCompName(ConnectorEnd conn) throws ConnectorException {
		String parts[] = conn.getPartWithPort().getHumanName().split(" ");
		return parts[parts.length - 1];
	}
	
	
	/**
	 * Returns the generic ID of the source port on this Connector.
	 * <p>
	 * This ID is not unique among different instances of the same module. 
	 * @param c Connector
	 * @return ID String
	 * @throws ConnectorException
	 */
	public static String getIDSourcePort(Connector c) throws ConnectorException {
		return getSourceConnEnd(c).getRole().getID();
	}

	/**
	 * Returns the generic ID of the target port on this Connector.
	 * <p>
	 * This ID is not unique among different instances of the same module. 
	 * @param c Connector
	 * @return ID String
	 * @throws ConnectorException
	 */
	public static String getIDTargetPort(Connector c) throws ConnectorException {
		return getTargetConnEnd(c).getRole().getID();
	}

	/**
	 * Returns the generic name of source port on this Connector.
	 * 
	 * @param c Connector
	 * @return Name
	 * @throws ConnectorException
	 */
	public static String getSourcePort(Connector c) throws ConnectorException {
		return getPortName(getSourceConnEnd(c));
	}
	
	/**
	 * Returns the generic name of target port on this Connector.
	 * 
	 * @param c Connector
	 * @return Name
	 * @throws ConnectorException
	 */
	public static String getTargetPort(Connector c) throws ConnectorException {
		return getPortName(getTargetConnEnd(c));
	}
	
	/**
	 * Returns the generic name of the role of the ConnectorEnd.
	 * 
	 * @param conn ConnectorEnd
	 * @return Name
	 * @throws ConnectorException
	 */
	public static String getPortName(ConnectorEnd conn) throws ConnectorException {
		String[] splitString = conn.getRole().getHumanName().split(" ");
		return splitString[splitString.length - 1];
	}
	
	/**
	 * Splits the in string with ' ' and returns the last values in the array.
	 * 
	 * @param in
	 * @return
	 */
	public static String getLastPartOfString(String in){
		String[] splitString = in.split(" ");
		return splitString[splitString.length - 1];
	}
	
	/**
	 * Returns a string with the qualified name of the base port it is attached to, along with the qualified name of the instance of the port (if it exists). 
	 * <p>
	 * This can be used to uniquely identify a port against other ports in a subsystem.
	 * 
	 * @param connEnd ConnectorEnd
	 * @return  String of the combined qualified name.
	 */
	public static String getQualifiedPort(ConnectorEnd connEnd) {
		String role = connEnd.getRole().getQualifiedName();
		String part = "";
		if (!connEnd.getRole().getObjectParent().getHumanType().equals("Subsystem")){
			part =  connEnd.getPartWithPort().getQualifiedName();
		}
		return role+"_"+part;
	}

	
	/**
	 * Returns the qualified name of the source port from the connector. 
	 * <p>
	 * The qualified name describes the port by providing the port's name, the module it is attached to, and the location of the module.
	 * 
	 * @param c Connector
	 * @return String qualified name
	 * @throws ConnectorException
	 */
	public static String getQualifiedSourcePort(Connector c) throws ConnectorException {
		ConnectorEnd connEnd = getSourceConnEnd(c);
		return getQualifiedPort(connEnd);
	}
	
	/**
	 * Returns the qualified name of the target port from the connector. 
	 * <p>
	 * The qualified name describes the port by providing the port's name, the module it is attached to, and the location of the module.
	 * 
	 * @param c Connector
	 * @return String qualified name
	 * @throws ConnectorException
	 */
	public static String getQualifiedTargetPort(Connector c) throws ConnectorException {
		ConnectorEnd connEnd = getTargetConnEnd(c);
		return getQualifiedPort(connEnd);
	}
	
	/**
	 * Returns the source port type from the Connector.
	 * 
	 * @param c Connector
	 * @return Port Type
	 * @throws ConnectorException
	 */
	public static String getSourcePortType(Connector c) throws ConnectorException {
		Type portType = getSourceConnEnd(c).getRole().getType();
		if (portType != null) {
			String typeString[] = portType.getHumanName().split(" ");
			return typeString[typeString.length - 1];
		} else {
			return "";
		}
	}

	/**
	 * Returns the target port type from the Connector.
	 * 
	 * @param c Connector
	 * @return Port Type
	 * @throws ConnectorException
	 */
	public static String getTargetPortType(Connector c) throws ConnectorException {
		Type portType = getTargetConnEnd(c).getRole().getType();
		if (portType != null) {
			String typeString[] = portType.getHumanName().split(" ");
			return typeString[typeString.length - 1];
		} else {
			return "";
		}
	}

	/**
	 * Checks the connectorEnd roles to match the returned value with the input (source) connector end.
	 * 
	 * This function does not check to see if both ConnectorEnds are different or have the same input/output types.
	 * 
	 * 
	 * @param c Connector
	 * @return Source ConnectorEnd
	 */
	public static ConnectorEnd getSourceConnEnd(Connector c) throws ConnectorException {
		return(getConnEnd(c , true));
	}
	
	/**
	 * Checks the connectorEnd roles to match the returned value with the output (target) connector end.
	 * 
	 * This function does not check to see if both ConnectorEnds are different or have the same input/output types.
	 * 
	 * 
	 * @param c Connector
	 * @return Target ConnectorEnd
	 */
	// Get the target end of a Connector
	public static ConnectorEnd getTargetConnEnd(Connector c) throws ConnectorException{
		return(getConnEnd(c , false));
	}
	
	/**
	 * Both getSourceConnEnd and getTargetConnEnd use this function. Incorporates error handling/checking.
	 * <p>
	 * The function checks the direction property of the first stereotype found in each port of the connector. 
	 * Depending on what the connector can see within and around the system, the function will return what it sees as the source and what is sees as the target.
	 * 
	 * @param c Connector
	 * @param isSource true if looking for source end, false if looking for target end
	 * @return Connector End
	 * @throws ConnectorException
	 */
	public static ConnectorEnd getConnEnd(Connector c , boolean isSource) throws ConnectorException {
		// getEnd returns 2 ConnectorEnd's: the first is the source, the second
		// the target end.
		ConnectorEnd out = null;
		int outCount = 0;
		int inCount = 0;
		
		int a = 0;
		int b = 1;
		
		if(!isSource){
			a = 1;
			b = 0;
		}

		//Iterate through both ports
		for(int i = 0; i != 2; i++){
			Port tempPort = null;
			//try casting the role into a Port. If it fails, the role doesn't not exist. 
			try{
				tempPort = (Port) c.getEnd().get(i).getRole();
			}
			catch(Exception e){
				Utils.throwConnectorException("Connector " + c.getName() + " in " + c.getObjectParent().getHumanName() + " has an end with a null role.");
			}
			
			if(tempPort == null){
				Utils.throwConnectorException("Connector " + c.getName() + " in " + c.getObjectParent().getHumanName() + " has an end with no role.");
			}
			
			//If the direction stereotype property is out
			try{
				if(((EnumerationLiteral) StereotypesHelper.getStereotypePropertyFirst(tempPort, StereotypesHelper.getFirstVisibleStereotype(tempPort) , "direction")).getName().equals("out")){
					outCount++;
					if(isSource){
						out = c.getEnd().get(i);
					}
					
				}
				//If the direction stereotype property is in
				else if(((EnumerationLiteral) StereotypesHelper.getStereotypePropertyFirst(tempPort, StereotypesHelper.getFirstVisibleStereotype(tempPort) , "direction")).getName().equals("in")){
					inCount++;
					if(!isSource){
						out = c.getEnd().get(i);
					}
				}
				else{
					Utils.throwConnectorException("Connector " + c.getName() + " in " + c.getObjectParent().getHumanName() + " has end " + tempPort.getQualifiedName() + " with a non input/output stereotype " +  StereotypesHelper.getFirstVisibleStereotype(tempPort) .getName()); //StereotypesHelper.findStereotypePropertyFor(tempPort , "direction").getDefaultValue().toString() + ".");
				}
			}
			catch(Exception e){
				Utils.throwConnectorException("Port in " + c.getObjectParent().getHumanName() + "  does not have direction. (The connector end does not have a sterotype with a direction value)");
			}
		}
	
		Property partWithPortZero = c.getEnd().get(0).getPartWithPort();
		Property partWithPortOne = c.getEnd().get(1).getPartWithPort();
		
		//If zero connected to the outside of the system
		if(partWithPortZero == null && partWithPortOne != null){
			//If both ports are input, the real source is coming from the outside of the system
			if(inCount == 2){
				out = c.getEnd().get(a);
			}
			//If both ports are outputs, the real source is coming from inside the system
			else if(outCount == 2){
				out = c.getEnd().get(b);
			}
			
		}
		//If one connected to the outside of the system
		else if(partWithPortZero != null && partWithPortOne == null){
			//If both ports are input, the real source is coming from the outside of the system
			if(inCount == 2){
				out = c.getEnd().get(b);
			}
			//If both ports are outputs, the real source is coming from inside the system
			else if(outCount == 2){
				out = c.getEnd().get(a);
			}
			
		}
		//If both have parts have the same "null value"
		else if(partWithPortZero == null && partWithPortOne == null || partWithPortZero != null && partWithPortOne != null){
			if(inCount == 2){
				Utils.throwConnectorException("Connector " + c.getName() + " in Subsystem " + c.getObjectParent().getHumanName() + " has two input ports connected (invalid).");
			}
			else if(outCount == 2){
				Utils.throwConnectorException("Connector " + c.getName() + " in Subsystem " + c.getObjectParent().getHumanName() + " has two output ports connected (invalid).");
			}
		}
		
		if(out == null){
			String errorMessage = "Connector " + c.getName() + " in Subsystem " + c.getObjectParent().getHumanName() + " with ends " + c.getEnd().get(0).getRole().getQualifiedName() + " and " + c.getEnd().get(1).getRole().getQualifiedName() + " does not have ";
			if(inCount == 0){
				errorMessage += "input ";
			}
			if(outCount == 0){
				errorMessage += "output "; 
			}
			Utils.throwConnectorException(errorMessage + "stereotypes.");
		}
		return out;
		
	}

	/**
	 * Returns a list of elements with the given stereotype String.
	 * 
	 * @param stereotype String
	 * @return List of Objects
	 */
	public static List<Element> getElementsOfStereotype(String stereotype) {
		Stereotype st;
		List<Element> elementList = new ArrayList<Element>();
		st = StereotypesHelper.getStereotype(Application.getInstance().getProject(), stereotype);
		if (st != null) {
			elementList = Arrays.asList(StereotypesHelper.getExtendedElementsIncludingDerived(st).toArray(new Element[0])); //.getExtendedElements(st); 
		}
		return elementList;
	}

	/**
	 * Returns the name space of the element.
	 * 
	 * @param componentElement
	 * @return
	 */
	public static String getNamespace(Element componentElement) {
		List<String> propvals;
		propvals = StereotypesHelper.getStereotypePropertyValueAsString(
				componentElement, "active", "Namespace");
		if (propvals.size() > 0) {
			return propvals.get(0);
		} else {
			propvals = StereotypesHelper.getStereotypePropertyValueAsString(
					componentElement, "passive", "Namespace");
			if (propvals.size() > 0) {
				return propvals.get(0);
			} else {
				propvals = StereotypesHelper
						.getStereotypePropertyValueAsString(componentElement,
								"queued", "Namespace");
				if (propvals.size() > 0) {
					return propvals.get(0);
				}
			}
		}
		return "";
	}

}
