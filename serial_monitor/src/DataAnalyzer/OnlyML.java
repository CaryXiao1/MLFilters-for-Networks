package DataAnalyzer;
import java.io.File;  // Import the File class
import java.io.FileNotFoundException;  // Import this class to handle errors
import java.util.ArrayList;
import java.util.Scanner; // Import the Scanner class to read text files
import java.io.FileWriter;   // Import the FileWriter class
import java.io.IOException;

public class OnlyML {

	public static void main(String[] args) throws IOException {
		boolean skip = false;
		ArrayList<String> toPrint = new ArrayList<String>(); 
		try {
			
			File file = new File("C:\\Users\\gameb\\OneDrive\\Desktop\\Docs\\Science Fair\\2021\\Testing Data\\750_bezos_reduced.txt");
			File newFile = new File("C:\\Users\\gameb\\OneDrive\\Desktop\\Docs\\Science Fair\\2021\\Testing Data\\750_bezos_onlyML_new.txt");
			FileWriter myWriter = new FileWriter(newFile);
			Scanner scanner = new Scanner(file);
			
			
			while(scanner.hasNextLine()) {
		        String data = scanner.nextLine();
		        toPrint.add(data);
		        
		        // resets per entry
		        if (data.length() >= 5 && data.substring(0,5).equals("-----")) {
		        	skip = true;
		        	for (int i = 0; i < toPrint.size(); i++) {
		        		// determines whether data has error
		        		String test = toPrint.get(i);
		        		if (test.length() >= 1 && test.substring(0,1).equals("L")) {
		        			if (test.length() >= 21 && test.substring(0, 22).equals("Locations of Error: {}")) skip = true;
		        			else skip = false;
		        		}
		        		
		        	}
		        	
		        	if (!skip) {
		        		for (int i = 0; i < toPrint.size(); i++) {
		        			System.out.println("----> " + toPrint.get(i));
		        			
		        			if (!toPrint.get(i).equals("")) myWriter.write(toPrint.get(i) + "\n");
		        			}
		        	}
		        	else {
		        		for (int i = 0; i < toPrint.size(); i++) {
		        			System.out.println(toPrint.get(i));
		        			}
		        	}
		        	
		        	// resets list
		        	toPrint = new ArrayList<String>(); 
		        }
			}
			myWriter.close();
			System.out.println("Successfully wrote to the file.");
			
		} catch (FileNotFoundException e) {
		      System.out.println("File not found.");
		      e.printStackTrace();
		}
		
		
	}
}
