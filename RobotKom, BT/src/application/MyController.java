package application;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.ListView;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;

public class MyController {
	
	@FXML
    private TextField sendTextField;
	
	@FXML
    private TextArea showSpeed;
	
	@FXML
    private ListView<String> myListView;
	
	private String speed;
	
	private PLabSerial serialConnection = new PLabSerial();;
	
	@FXML
	void initialize() {
		listSerialPorts();
		serialConnection.addListener(this, "messageReceived");
	}
	

	
	public String getSpeed() {
		return speed;
	}
	
	public void showSpeed() {
		showSpeed.appendText("Current speed is: " + getSpeed() +  "\r\n");
	}
	
	@FXML
	void listSerialPorts() {
	    String[] portNames = serialConnection.getPortNames();
		ObservableList<String> items =FXCollections.observableArrayList (portNames);
		myListView.setItems(items);	
	}
	
	@FXML
	void openSelectedPort() {
		int index = myListView.getSelectionModel().getSelectedIndex(); 
		if (index > -1) {
		  String portName = myListView.getSelectionModel().getSelectedItem();
		  serialConnection.openPort(portName);		
		}
		
	} 
	
	@FXML
	void openPLabPort() {
		serialConnection.openPLabPort();	
		String portName = serialConnection.getOpenPortName();
		String[] portNames = serialConnection.getPortNames();
		int i = java.util.Arrays.asList(portNames).indexOf(portName);
		if (i>-1) {
			myListView.getSelectionModel().select(i);		
		}
	} 
	
	@FXML
	void closePort() {
		serialConnection.closePort();
	}
	
	@FXML
	void sendText() {	
	    String s = getSpeed();
	    serialConnection.sendMessage(s);
	}
	
	@FXML
	void strat1() {
		this.speed = "Strat, 100";
		showSpeed();
	}
	
	@FXML
	void strat2() {
		this.speed = "Strat, 400";
		showSpeed();
	}
	
	@FXML
	void strat3() {
		this.speed = "Strat, 250";
		showSpeed();
	}
	
}
