package gov.nasa.jpl.componentaction;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import com.nomagic.magicdraw.core.Application;
import com.nomagic.uml2.ext.jmi.helpers.StereotypesHelper;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Element;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.MultiplicityElement;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.NamedElement;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Type;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.TypedElement;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Package;


/**
 * 
 * This class encapsulates information pertaining to the component types and their ports in the MD model - ie active, passive or queued components 
 *
 */
public class ISFComponent {
	private Element componentElement;
	private String compStereotype;
	private String URIName;   // Parent package URI
	// private List<Element> portList;
	HashMap<String, portObject> filtList;
	String IncludeCmdXML;
	String IncludeTlmXML;
	String IncludeEvrXML;
	String IncludeParamXML;
	String IncludeInternalIFXML;
	String IncludeIncludeHdr;

	/**
	 * Used to process ports in the component phase.
	 */
	public class portObject {
		
		portObject() {
			index = 0;
			stereotype = "";
			datatype = "";
			datatypeNamespace = "";
			role = "";
			priority = "";
			full = "";
		}

		public int getIndex() {
			// Return the port multiplicity
			return index;
		}

		public String getStereotype() {
			return stereotype;
		}

		public String getDatatype() {
			return datatype;
		}

		public String getDatatypeNamespace() {
			return datatypeNamespace;
		}
		
		public String getRole() {
			return role;
		}
		
		public String getPriority() {
			return priority;
		}
		
		public String getFull() {
			return full;
		}

		int index;
		String stereotype;
		String datatype;
		String datatypeNamespace;
		String role;
		String priority;
		String full;
	}

	/**
	 * A base class for all Ports that do not specify a data type but have a
	 * reference to some existing xml data structure 
	 */
	public class referencePort {
		referencePort() {
			Reference = "";
		}

		public String getReference() {
			return Reference;
		}

		String Reference;
	}

	/**
	 * Instantiation for the ISFComponent. 
	 * 
	 * @param componentElement The element of the component that will be processed.
	 */
	public ISFComponent(Element componentElement) {
		this.componentElement = componentElement;
		// portList = new ArrayList<Element>();
		filtList = new HashMap<String, portObject>();
		IncludeCmdXML = getComponentProperty(componentElement, "IncludeCmdXML");
		IncludeTlmXML = getComponentProperty(componentElement, "IncludeTlmXML");
		IncludeEvrXML = getComponentProperty(componentElement, "IncludeEvrXML");
		IncludeParamXML = getComponentProperty(componentElement, "IncludeParamXML");
		IncludeInternalIFXML = getComponentProperty(componentElement, "IncludeInternalIFXML");
		IncludeIncludeHdr = getComponentProperty(componentElement, "IncludeHdrFile");

		Package p = (Package) componentElement.getObjectParent();
		URIName = p.getURI();
	}

	/**
	 * Sets the component stereotype string value
	 * @param compStereotype Stereotype string
	 */
	public void setStereoType(String compStereotype) {
		this.compStereotype = compStereotype;
	}

	/**
	 * Creates a new port object using the port Element inputed. This processes the port and can throw an exception if something in the port is illegal.
	 * It also adds the port into fillList. 
	 * 
	 * <p>
	 * 
	 * 
	 * @param portElement
	 * @throws PortException
	 */
	public void newAddPort(Element portElement) throws PortException {
		portObject port = new portObject();
		String portName = portElement.getHumanName().split(" ")[1];
		port.index = ((MultiplicityElement) portElement).getLower();
		port.datatype = getPortDataType(portElement);
		port.datatypeNamespace = getPortDataTypeNamespace(portElement);
		port.stereotype = getPortStereotype(portElement);
		filtList.put(portName, port);
		if (port.stereotype.equals("async_input")) {
			port.priority = getPortProperty(portElement, "priority");
			port.full = getPortProperty(portElement, "full");
		}
	}
	
	/**
	 * Creates a new port object using the port Element inputed. This processes the port and can throw an exception if something in the port is illegal.
	 * It also adds the port into fillList.
	 * 
	 * @param portElement
	 * @throws PortException
	 */
	public void newRolePort(Element portElement) throws PortException {
		portObject port = new portObject();
		String portName = portElement.getHumanName().split(" ")[1];
		port.index = ((MultiplicityElement) portElement).getLower();
		port.datatype = getPortDataType(portElement);
		port.datatypeNamespace = getPortDataTypeNamespace(portElement);
		//port.stereotype = getPortStereotype(portElement);
		if(StereotypesHelper.hasStereotypeOrDerived(portElement, "input")) {
			port.stereotype = "input";
		} else if(StereotypesHelper.hasStereotypeOrDerived(portElement, "output")) {
			port.stereotype = "output";
		}
		port.role = getPortStereotype(portElement);
		filtList.put(portName, port);
	}

	/**
	 * Returns the port name.
	 * 
	 * @param portElement port which the name will be extracted from
	 * @return The name of the port
	 */
	public String getPortName(Element portElement) {
		return portElement.getHumanName().split(" ")[1];
	}

	/**
	 * Returns the port stereotype.
	 * 
	 * @param portElement port which the stereotype will be extracted from
	 * @return The stereotype of the port
	 */
	public String getPortStereotype(Element portElement) {
		return portElement.getHumanName().split(" ")[0];
	}
	
	
	/**
	 * This returns the full path of the data type which includes the directory tree.
	 * 
	 * @param portElement
	 * @return port data type string
	 * @throws PortException
	 */
	public String getPortDataType(Element portElement) throws PortException {
		// Perform a null check
		if(((TypedElement) portElement).getType() == null) {
			String errstr = "Port " + ((TypedElement)portElement).getName() + " of component " 
					+ ((NamedElement)(portElement.getOwner())).getName() + " has no data type";
			throw new PortException(errstr);
		}
		// Get the parent package of the port interface type
		Package p = (Package) ((TypedElement)portElement).getType().getObjectParent();
		// Get the packages URI name
		String portURIName = p.getURI();
		return portURIName + "/" + ((TypedElement)portElement).getType().getName();
	}


	/**
	 * This returns the port data type without the full path.
	 * It also tags on the Namespace.
	 * 
	 * @param portElement
	 * @return data type + '::' + name space
	 */
	public String getPortDataTypeNamespace(Element portElement) {	
		String namespace = "";
		Type portType = ((TypedElement) portElement).getType();
		String propVal = getPortTypeProperty(portType, "Namespace");
        if (!propVal.equals("")) {
			namespace = propVal + "::";
		}
		String[] portName = portType.getHumanName().split(" ");
		return namespace + portName[portName.length - 1];
	}

	/**
	 * Returns the filtList HashMap, a map with a string associated with a port object.
	 * @return HashMap
	 */
	public HashMap<String, portObject> getPortHashList() {
		return filtList;
	}
	
	
	/**
	 * Returns the component's URI, which is the location in the tree hierarchy.
	 * @return string of position in hierarchy
	 */
	public String getURI() {
		return URIName;
	}
	

	/**
	 * Return the full dictionary path which includes the URI path specified in the package above the component.
	 * @param includeString 
	 * @return input string prepended by URIName
	 */
	private String getDictionaryString(String includeString) {
		if (includeString.equals("")) {
			   return "";
			} else {
				if (URIName.equals("")) {
					return includeString;
				}
				else {
				    return URIName + "/" + includeString;
				}
			}
	}
	
	/**
	 * Prepends the IncludeCmdXML String  with the URI Name
	 * @return address string
	 */
	public String getIncludeCmdXML() {
		return getDictionaryString(IncludeCmdXML);
	}
	
	/**
	 * Prepends the IncludeEvrXML String  with the URI Name
	 * @return address string
	 */
	public String getIncludeEvrXML() {
		return getDictionaryString(IncludeEvrXML);
	}
	
	/**
	 * Prepends the IncludeTlmXML String  with the URI Name
	 * @return address string
	 */
	public String getIncludeTlmXML() {
		return getDictionaryString(IncludeTlmXML);
	}
	
	/**
	 * Prepends the IncludeParamXML String  with the URI Name
	 * @return address string
	 */
	public String getIncludeParamXML() {
		return getDictionaryString(IncludeParamXML);
	}

	/**
	 * Prepends the IncludeInternalIFXML String  with the URI Name
	 * @return address string
	 */
	public String getIncludeInternalIFXML() {
		return getDictionaryString(IncludeInternalIFXML);
	}
	
	/**
	 * Prepends the IncludeIncludeHdr String  with the URI Name
	 * @return address string
	 */
	public String getIncludeHdrFile() {
		return getDictionaryString(IncludeIncludeHdr);
	}
	
	/**
	 * Get the component stereotype attribute called "Namespace"
	 * @return Namespace string of the componentElement
	 */
	public String getNamespace() {
		List<String> propvals = StereotypesHelper
				.getStereotypePropertyValueAsString(componentElement,
						getStereotype(), "Namespace");
		if (propvals.size() > 0) {
			return propvals.get(0);
		} else {
			return "";
		}
	}

	/**
	 * Returns last part of the componentElement name
	 * @return name
	 */
	public String getName() {
		return componentElement.getHumanName().split(" ")[1];
	}
	
	/**
	 * Returns compStereotype
	 * @return the stereotype of the object
	 */
	public String getStereotype() {
		return compStereotype;
	}

	/**
	 * Creates a list of strings based off the data type from filtList values
	 * @return A list of data type strings 
	 */
	public List<String> getPortDataList() {
		List<String> dataList = new ArrayList<String>();
		for (portObject e : filtList.values()) {
			String dataType = e.datatype;
			if (!dataList.contains(dataType)) {
				dataList.add(dataType);
			}
		}
		return dataList;
	}
	
	/**
	 * Returns the property value of PortType stereotype of the input element argument.
	 * @param portElement Element of the port type
	 * @param property String of value to be looked for in the stereotype PortType
	 * @return The value of the attribute  from property
	 */
	public String getPortTypeProperty(Element portElement, String property) {
		List<String> propvals = StereotypesHelper
				.getStereotypePropertyValueAsString(portElement, "PortType", property);
		if (propvals.size() > 0) {
			return propvals.get(0);
		} else {
			return "";
		}
	}
	
	
	/**
	 * Returns the property value of Component stereotype of the input element argument.
	 * @param portElement Element of the port type
	 * @param property String of value to be looked for in the stereotype Component
	 * @return The value of the attribute  from property
	 */
	public String getComponentProperty(Element portElement, String property) {
		List<String> propvals = StereotypesHelper
				.getStereotypePropertyValueAsString(portElement, "Component", property);
		if (propvals.size() > 0) {
			return propvals.get(0);
		} else {
			return "";
		}
	}
	
	/**
	 * Returns the property value of async_input stereotype of the input element argument.
	 * @param portElement Element of the port type
	 * @param property String of value to be looked for in the stereotype async_input
	 * @return The value of the attribute  from property
	 */
	public String getPortProperty(Element portElement, String property) {
		List<String> propvals = StereotypesHelper
				.getStereotypePropertyValueAsString(portElement, "async_input", property);
		if (propvals.size() > 0) {
			return propvals.get(0);
		} else {
			return "";
		}
	}

	/**
	 * 
	 */
	public void print() {
		//System.out.println(compStereotype + " " + getName());
		//Application.getInstance().getGUILog()
		//		.log(compStereotype + " " + getName());
		// for (Element e : portList) {
		// System.out.println("port stereotype = " + getPortStereotype(e)
		// + ", name = " + getPortName(e) + ", datatype = "
		// + getPortDataType(e));
		// }
	}
	
	/**
	 * All passive components cannot have async_input ports
	 * All active components should have sync_input ports
	 * All queued components should have sync and async input ports
	 * @throws ComponentException
	 */
	public void checkComponent() throws ComponentException {
		String compName = ((NamedElement)componentElement).getName();
		if(!isValid("passive", "async_input", false) || !isValid("Passive Component", "async_input", false) || !isValid("passive", "Async", false) || !isValid("Passive Component", "Async", false)) {
			String errStr = "Passive component " + compName + " cannot have asynchronous input ports";
			throw new ComponentException(errStr);
		}
		
		/*
		if(!isValid("active", "async_input", true)) {
			String errStr = "===>Warning: Active component " + compName + " should have at least one asynchronous input port";
			warnLog(errStr);
		}
		
		
		if(!isValid("queued", "sync_input", true)) {
			String errStr = "===>Warning: Queued component " + compName + " should have at least one synchronous input port";
			warnLog(errStr);
		} 
		*/
	}
	
	/**
	 * Prints a warning to the Java standard out as well as the MagicDraw console.
	 * @param errStr Message to be printed
	 */
	public void warnLog(String errStr) {
		System.out.println(errStr);
		Application.getInstance().getGUILog().log(errStr);
	}
	
	/**
	 * Checks if a component is of a specific stereotype
	 * If so, checks if the component satisfies the requirement for a specific port type
	 * @param stereotype	the stereotype the component should have
	 * @param portType		the port type to check for
	 * @param atLeastOne	if the component should have the port type
	 */
	public boolean isValid(String stereotype, String portType, boolean atLeastOne) {
		if(!StereotypesHelper.hasStereotype(componentElement, stereotype)) {
			return true;
		}
		else {
			for(String name : filtList.keySet()) {
				portObject port = filtList.get(name);
				//System.out.println("Port: " + name + ", Stereotype: " +port.stereotype);
				if(port.stereotype.equals(portType)) {
					if(atLeastOne) {
						return true;
					}
					else {
						return false;
					}
				}
			}
			if(atLeastOne) {
				return false;
			}
			else {
				return true;
			}
		}
	}
}
