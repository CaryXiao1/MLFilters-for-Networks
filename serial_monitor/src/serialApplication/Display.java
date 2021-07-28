package serialApplication;
import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.PrintWriter;
import java.util.Scanner;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.UIManager;

import com.fazecast.jSerialComm.SerialPort;

import weka.core.Instances;
import weka.classifiers.functions.SGDText;
/** This class was created by Cary Xiao on January 16th, 2021.
 *  Functions as GUI / Driver Class for application.
 * 
 * 
 */
public class Display {
	
	static SerialPort chosenPort;
	
	public static void main(String[] args) throws Exception {
		// Tries to get OS' specific layout
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Exception e) {}
		
		Font font = new Font("Tahoma", Font.PLAIN, 16);
		
		// Create and Configure the application
		JFrame window = new JFrame();
		window.setTitle("Serial Port Application");
		window.setSize(1500, 900);
		window.setLayout(new BorderLayout());
		window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		// Create a drop-down box & connect button, then place @ top of window
		JComboBox<String> portList = new JComboBox<String>();
		portList.setFont(font);
		JButton connectButton = new JButton("Connect");
		connectButton.setFont(font);
		
		JPanel topPanel = new JPanel();
		topPanel.add(portList);
		topPanel.add(connectButton);
		window.add(topPanel, BorderLayout.NORTH);
		// populate drop-down box
		SerialPort[] portNames = SerialPort.getCommPorts();
		for (int i = 0; i < portNames.length; i++) {
			portList.addItem(portNames[i].getDescriptivePortName());
		}
		
		// Create SerialPanel & place @ center of window
		SerialPanel serialPanel = new SerialPanel(font);

		
		// Create inputPanel & place @ bottom of window
		JPanel inputPanel = new JPanel(new FlowLayout());
		JTextField input = new JTextField(45);
		input.setFont(font);
		input.setEnabled(false);
		JButton send = new JButton("Send");
		send.setFont(font);
		send.setEnabled(false);
		
		inputPanel.add(input);
		inputPanel.add(send);
		window.add(inputPanel, BorderLayout.SOUTH);
		
		// configure connect button and use thread to listen for data
		connectButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				if (connectButton.getText().equals("Connect")) {
					// attempt to connect to Serial Port
					String selectedItem = portList.getSelectedItem().toString();
					selectedItem = selectedItem.substring(selectedItem.length() - 5, selectedItem.length() - 1);
					System.out.println(selectedItem);
					chosenPort = SerialPort.getCommPort(selectedItem);
					chosenPort.setComPortTimeouts(SerialPort.TIMEOUT_SCANNER, 0, 0);
					
					if (chosenPort.openPort()) {
						connectButton.setText("Disconnect");
						portList.setEnabled(false);
						input.setEnabled(true);
						send.setEnabled(true);
						window.add(serialPanel, BorderLayout.CENTER);
					}
					
					// create new thread that listens for incoming information
					Thread thread = new Thread() {
						@Override 
						public void run() {
							Scanner scanner = new Scanner(chosenPort.getInputStream());
							while(scanner.hasNextLine()) {
								String line = scanner.nextLine();
								System.out.println(line);
								serialPanel.addData(line);
							}
							scanner.close();
							
						}
						
					};
					thread.start();
				}
				else {
					chosenPort.closePort();
					portList.setEnabled(true);
					connectButton.setText("Connect");
					input.setEnabled(false);
					send.setEnabled(false);
				}
			}
			
		});
		
		// configure send button & input text field
		send.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				String sendString = input.getText();
				if (sendString != "") {
					PrintWriter output = new PrintWriter(chosenPort.getOutputStream());
					output.print(sendString);
					output.flush();
					input.setText("");
				}
				
			}
		});
		input.addActionListener(send.getActionListeners()[0]);

		// show the window
		window.setVisible(true);
	}
}
