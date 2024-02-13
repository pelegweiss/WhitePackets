# WhitePackets

WhitePackets is a feature-rich packet sniffer and simulator for MapleStory V83, offering a range of capabilities to enhance packet analysis and manipulation.

## Features

- **Packet Sniffing:** Capture and analyze network packets between the MapleStory client and server.
- **Packet Simulation:** Simulate custom packets to manipulate the game's communication flow.
- **Dynamic DLL Injection:** Easily inject the required DLL into the MapleStory process by specifying the game's path and the DLL's path. The built-in DLL injector streamlines the injection process, eliminating the need for external tools.
- **Path Configuration:** Configure paths for the game and DLL directly through the GUI, simplifying setup and ensuring a smooth user experience.
- **Byte Group Colorization:** Enhance readability by presenting raw strings instead of byte representations and colorizing different groups of bytes related to each other. Each color corresponds to a specific group, providing a visual representation of packet structures.
- **Filtering:** Prevent the display of specific packets with a given header, allowing users to focus on relevant information.
- **Blocking:** Prevent the client from handling incoming packets with a particular header specified by the user, providing control over packet processing.
- **Filter Management:** Save and load filters for convenient and efficient usage in future sessions.

## HeavenMS

WhitePackets was developed and tested in conjunction with the [HeavenMS](https://www.google.com/search?q=heavenms&rlz=1C1GCEA_enIL1061IL1061&oq=heavenms&gs_lcrp=EgZjaHJvbWUyBggAEEUYOTINCAEQLhiDARixAxiABDIGCAIQIxgnMgoIAxAuGNQCGIAEMgoIBBAuGNQCGIAEMgYIBRBFGDwyBggGEEUYPTIGCAcQRRg90gEIMzAxM2owajeoAgCwAgA&sourceid=chrome&ie=UTF-8) project. HeavenMS provides a private MapleStory server (v83) that served as a sandbox environment for testing and developing WhitePackets.

## Installation

1. **Download Executable:**
   - Get the latest executable from the [releases page](link-to-releases).

2. **Run Executable:**
   - Launch the executable to start the MapleStory game with packet sniffing and simulation capabilities.

3. **DLL Injection:**
   - The executable injects a DLL into the game for packet interception and simulation.

## Usage

1. **GUI Interface:**
   - The executable serves as the GUI, presenting all intercepted packets.
   - Users can input packets to simulate through the GUI.

2. **Packet Interception:**
   - The DLL intercepts packets by manipulating game memory.

3. **Packet Simulation:**
   - Simulate packets by injecting custom data into the client/server communication.

## Build

To build your executable (exe) and dynamic link library (DLL), follow these steps:

1. **Clone the Project:**
   - Clone this repository to your local machine.

```bash
git clone https://github.com/your-username/WhitePackets.git
```
2. **Open Solution Files:**

   - Navigate to the cloned directory and open the following solution files using Visual Studio or your preferred IDE:

   - WhitePackets/DLL/WhitePackets.sln: Solution file for the DLL.

   - WhitePackets/GUI/GUI.sln: Solution file for the executable (GUI).
3. **Set Additional Library Directories:**
   - Open the WhitePackets & GUI solution in Visual Studio.
   - Navigate to Project Properties -> Configuration Properties -> VC++ Directories.
   - Set "$(SolutionDir)\Dependencies" for the "Additional Library Directories" parameter.

4.**Compile for x86:**
    - For both solutions, ensure that you are compiling for the target machine architecture of x86.

**Build the solutions to generate the executable and DLL.**

## Dependencies

- [Pipe Library](https://github.com/pelegweiss/Pipe): Custom library for communication between the executable and DLL using named pipes.


## Setup Instructions

1. **Configure Executable:**
   - Ensure the executable is set up with the correct paths and settings.

2. **Run MapleStory:**
   - Launch MapleStory using the executable.

## Contributing

If you'd like to contribute to WhitePackets, please follow these guidelines:
- Fork the repository.
- Create a new branch for your feature or bug fix.
- Make your changes and submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).

## Disclaimer

WhitePackets is for educational purposes only. Use responsibly and adhere to MapleStory's terms of service.

## Support

For any issues or questions, please [open an issue](link-to-issues) on GitHub.
