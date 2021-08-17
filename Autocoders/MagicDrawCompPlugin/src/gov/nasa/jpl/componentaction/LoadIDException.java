package gov.nasa.jpl.componentaction;
/**
 *  Shell exception for the LoadIDConfig class. This just serves as a wrapper that can be used to differentiate between where an error happens.
 *
 */
public class LoadIDException extends Exception {
   public LoadIDException(String message) {
	   super(message);
   }
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

}
