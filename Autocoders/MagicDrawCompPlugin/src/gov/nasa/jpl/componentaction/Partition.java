package gov.nasa.jpl.componentaction;


import com.nomagic.magicdraw.core.Application;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Element;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.TypedElement;

/**
 * Creates the partition object.
 */
public class Partition {
	private Element partitionElement;

	/**
	 * Assigns the input partitionElement to the instance variable partitionElement.
	 * @param partitionElement
	 */
	public Partition(Element partitionElement) {
		this.partitionElement = partitionElement;
	}
		
	/**
	 * Returns the name of the partitionElement.
	 * @return name string
	 */
	public String getName() {
		return partitionElement.getHumanName();
	}
	
	/**
	 * Returns the third word in the name of the dataElement if the name was divided by spaces.
	 * @param dataElement
	 * @return the name of the dataElement
	 */
	public String getDataName(Element dataElement) {
		return dataElement.getHumanName().split(" ")[2];
	}
	
	/**
	 * Returns the third word in the type of the dataElement if the type was divided by spaces.
	 * @param dataElement
	 * @return the type of the dataElement
	 */
	public String getDataType(Element dataElement) {
		   TypedElement te = (TypedElement)dataElement;
		   return te.getType().getHumanName().split(" ")[2];
	}
		
	/**
	 * Prints the partition name to Java standard out and the MagicDraw console.
	 */
	public void print() {
		System.out.println("Partition Name: " + getName());
		Application.getInstance().getGUILog().log("Partition Name: " + getName());
	}
}
