

gphoto2

--filename path
--debug-logfile FILENAME 


-f, --folder FOLDER
// Specify camera folder (default="/").


--capture-preview
// Capture a quick preview.


-F COUNT, --frames COUNT
// Number of frames to capture in one run. Default is infinite number of frames.

-I SECONDS, --interval SECONDS
// Time between capture of multiple frames.

// (Since 2.4) If SIGUSR1 signal is received, a picture is taken immediately without waiting for the end of the current interval period (see the section called “Signals”). A value of -1 will let gphoto2 wait forever, i.e. until a signal arrives. See also --reset-interval (since 2.4).




--reset-interval (since 2.4)
// Setting this option will reset the time interval to the value given by the -I|--interval option when a SIGUSR1 signal is received in time-lapse mode.

--capture-image
// Capture an image and keep it on the camera.

--trigger-capture
// Trigger the capture an image and return. If you want to get the image downloaded, see --wait-event-and-download

--capture-image-and-download
// Capture an image and download it immediately to the computer.


--keep
// When doing --capture-image-and-download or interval capture, this option will keep the images on the memory card of the camera.

--no-keep
// When doing --capture-image-and-download or interval capture, this option will not keep the images on the memory card of the camera after downloading them during capture. (default)

--capture-movie
// Capture a movie. If the camera supports previews, this will capture a stream of previews (motion-jpeg) as fast as the camera can.


--capture-sound
// Capture an audio clip. (No driver supports this at this time.)



--capture-tethered
/*Lets gphoto2 wait for notifications from the camera that an object was added. This is useful for tethered capture, where pressing the shutter on the camera immediately transfer the image to the machine for processing.

Together with the --hook-script to immediately postprocess or display the images this can help a studio workflow.

This option requires support in the driver and by the camera, currently newer Canon EOS and Nikon DSC are known to work.


*/




--wait-event SECONDS or COUNT, --wait-event-and-download SECONDS or COUNT
/*Lets gphoto2 wait for notifications from the camera for various things. This is useful for seeing what the camera does and waiting for objects to be added. The objects are kept on camera in the “--wait-event” version, with “--wait-event-and-download” they are downloaded.
*/
--wait-event-and-download is equivalent to --capture-tethered.
/*
The time to wait can be either specified as full seconds with a "s" suffix, or as a number of events (just a number). If nothing happens, a timeout is generated after 1 second, so a “wait-event=5” will take at most 5 seconds. A “--wait-event=5s” will take exactly 5 second.

If no argument is given, the wait time is 1 million events (basically forever).

In the download variant this can be used together with the --hook-script to immediately postprocess or display the images this can help a studio workflow.

This option requires support in the driver and by the camera, currently newer Canon EOS and Nikon DSC are known to work.*/



--list-config-
--get-config CONFIGENTRY
--set-config CONFIGENTRY=CONFIGVALUE
--set-config-index CONFIGENTRY=CONFIGINDEX
--set-config-value CONFIGENTRY=CONFIGVALUE

--manual
Camera driver manual.

--shell
Start the gphoto2 shell, an interactive environment. See SHELL MODE for a detailed description.







--hook-script FILENAME (new after 2.3.0)
Execute the hook script FILENAME every time a certain event happens within gphoto2. The hook script reads the environment variable ACTION. It must ignore ACTION values unknown to it.
ACTION=init


Default value for this option can be specified in the ~/.gphoto/settings file as gphoto2=filename=value.
