package gov.nasa.jpl.componentaction;

import java.util.ArrayList;
import java.util.List;

import com.nomagic.uml2.ext.jmi.helpers.StereotypesHelper;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Element;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Enumeration;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.EnumerationLiteral;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Operation;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Type;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.TypedElement;

/* This class encapsulates information pertaining to the InterfaceBlock stereotype in the MD model */
public class ISFPort {
	private Element portElement;
	private List<Element> dataList; // arguments in the Port Interface
	private Element returnVal; // Port Interface return value
	private List<String> includeList; // List of include header files

	/**
	 * Creates an ISFPort object 
	 * @param portElement Element of port type to be used in the creation of the object
	 */
	public ISFPort(Element portElement) {
		this.portElement = portElement;
		dataList = new ArrayList<Element>();
		includeList = new ArrayList<String>();
		returnVal = null;

		//for (Comment c : portElement.get_commentOfAnnotatedElement()) {
			//System.out.println("--- comment: " + c.getBody());
		//}
	}

	/**
	 * Return the port element's namespace
	 * @return namespace string
	 */
	public String getNamespace() {
		return getPortTypeProperty(portElement, "Namespace");
	}

	/**
	 * Adds Element to the dataList and checks to make sure the datatype is not empty before adding it to the includeList.
	 * @param dataElement Element to be added
	 */
	public void addData(Element dataElement) {
		this.dataList.add(dataElement);
		Type dataType = ((TypedElement) dataElement).getType();
		
		String includefile = getDataTypeProperty(dataType, "Include");
		if (!includefile.equals("")) {
			includeList.add(includefile);
		}
	}

	/**
	 * Adds Element to the includeList if the datatype is not empty.
	 * @param dataElement
	 */
	public void addReturn(Element dataElement) {
		Type dataType = ((Operation) dataElement).getType();
		String includefile = getDataTypeProperty(dataType, "Include");
		if (!includefile.equals("")) {
			includeList.add(includefile);
		}		
		this.returnVal = dataElement;
	}
	
	/**
	 * Checks if the Element object is an instance of Enumeration.
	 * @param dataElement Object to be checked
	 * @return True if the Element is an instance of Enumeration
	 */
	public boolean isReturnEnumeration(Element dataElement) {
		return (((Operation) dataElement).getType() instanceof Enumeration);
	}

	/**
	 * Checks if the returnVal is null.
	 * @return True if the ISFPort has a returnValue
	 */
	public boolean hasReturn() {
		return (returnVal != null);
	}

	/**
	 * Returns the returnVal.
	 * @return returnVal
	 */
	public Element getReturn() {
		return returnVal;
	}

	/**
	 * Returns String based off the dataElement.
	 * @param dataElement Input Element
	 * @return ReturnType
	 */
	public String getReturnType(Element dataElement) {
		Type dataType = ((Operation) dataElement).getType();
		String namespace = "";
		
		String namespaceProp = getDataTypeProperty(dataType, "Namespace");
		if (!namespaceProp.equals("")) {
		   namespace = namespaceProp + "::"; 
		}
		String[] parts = dataType.getHumanName().split(" ");
		return namespace + parts[parts.length - 1];
	}

	/**
	 * Returns the last part of the object name.
	 * @return String name
	 */
	public String getName() {
		String[] parts = portElement.getHumanName().split(" ");
		return parts[parts.length - 1];
	}

	/**
	 * Checks if the dataElement is a reference or pointer and returns a list of Strings where the first value is the pass type and the second value is the name of the dataElement.
	 * @param dataElement
	 * @return List of Strings
	 */
	public String[] getDataName(Element dataElement) {
		String[] parts = dataElement.getHumanName().split(" ");
		String retString[] = new String[2];
		String nameString = parts[parts.length - 1];

		// Check for the pass by Reference type modifier
		if (nameString.substring(0, 1).equals("&")) {
			retString[0] = nameString.substring(1);
			retString[1] = "reference";
			return retString;
		}

		// Check for the pass by Pointer type modifier
		if (nameString.substring(0, 1).equals("*")) {
			retString[0] = nameString.substring(1);
			retString[1] = "pointer";
			return retString;
		}

		else {
			retString[0] = nameString;
			retString[1] = "value";
			return retString;
		}

	}

	/**
	 * Returns the data type with the namespace property prepended to it.
	 * @param dataElement 
	 * @return namespace String
	 */
	public String getDataType(Element dataElement) {
		Type dataType = ((TypedElement) dataElement).getType();
		String namespace = "";
		
		String namespaceProp = getDataTypeProperty(dataType, "Namespace");
		if (!namespaceProp.equals("")) {
		   namespace = namespaceProp + "::"; 
		}
		String[] parts = dataType.getHumanName().split(" ");
		return namespace + parts[parts.length - 1];
	}

	/**
	 * Returns the dataLIst
	 * @return List of Elements
	 */
	public List<Element> getDataList() {
		return dataList;
	}

	/**
	 * Returns the includeList
	 * @return List of Strings
	 */
	public List<String> getIncludeList() {
		return includeList;
	}

	/**
	 * Returns the enumerated items of the datatype.
	 * @param dataElement
	 * @return List of Enumeration Literal objects
	 */
	public List<EnumerationLiteral> getEnumItems(Element dataElement) {
		return ((Enumeration) ((TypedElement) dataElement).getType())
				.getOwnedLiteral();
	}
	
	/**
	 * Get enumeration items from a Port's Enumeration return type
	 * @param dataElement
	 * @return
	 */
	public List<EnumerationLiteral> getReturnEnumItems(Element dataElement) {
		return ((Enumeration) ((Operation) dataElement).getType())
				.getOwnedLiteral();
	}

	/**
	 * Checks if the Type of the dataElement is an instance of Enumeration.
	 * @param dataElement
	 * @return boolean
	 */
	public boolean isEnumeration(Element dataElement) {
		return (((TypedElement) dataElement).getType() instanceof Enumeration);
	}
	
	
	/**
	 * Returns the property value of the PortType's stereotype of the input element argument.
	 * 
	 * @param dataElement The Element in question.
	 * @param property The parameter that is being accessed within the PortType stereotype
	 * @return the associated value of the property in the PortType stereotype
	 */
	public String getPortTypeProperty(Element dataElement, String property) {
		List<String> propvals = StereotypesHelper
				.getStereotypePropertyValueAsString(dataElement, "PortType", property);
		if (propvals.size() > 0) {
			return propvals.get(0);
		} else {
			return "";
		}
	}
	
	/**
	 * Returns the property value of DataType stereotype of the input element argument.
	 * 
	 * @param dataElement The Element in question.
	 * @param property The parameter that is being accessed within the DataType stereotype
	 * @return the associated value of the property in the DataType stereotype
	 */
	public String getDataTypeProperty(Element portElement, String property) {
		List<String> propvals = StereotypesHelper
				.getStereotypePropertyValueAsString(portElement, "DataType", property);
		if (propvals.size() > 0) {
			return propvals.get(0);
		} else {
			return "";
		}
	}
	
	
	/**
	 * Prints the name of the ISPort object.
	 */
	public void print() {
		//Application.getInstance().getGUILog().log("Port Name: " + getName());
		System.out.println("Port Name: " + getName());
		//for (Element e : dataList) {
		//	System.out.println("Data Element = " + getDataName(e)
		//			+ ", datatype = " + getDataType(e));
		//}
	}
}