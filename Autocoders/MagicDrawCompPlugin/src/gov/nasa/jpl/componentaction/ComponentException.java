package gov.nasa.jpl.componentaction;

/**
 * Component Exception shell class. This just serves as a wrapper that can be used to differentiate between where an error happens.
 *
 */
public class ComponentException extends Exception {
   public ComponentException(String message) {
	   super(message);
   }
	private static final long serialVersionUID = 1L;

}
