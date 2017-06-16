package gov.nasa.jpl.componentaction;
/**
 * Connector Exception shell class. This just serves as a wrapper that can be used to differentiate between where an error happens.
 *
 */
public class ConnectorException extends Exception {
   public ConnectorException(String message) {
	   super(message);
   }
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

}
