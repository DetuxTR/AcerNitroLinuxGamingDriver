# AcerNitroLinuxGamingDriver
An experimental driver to set fan speeds, and the keyboard rgb on linux.

Thanks to https://github.com/tuxedocomputers/tuxedo-drivers for a starting point to new device based wmi drivers on linux kernel.
Thanks to https://github.com/JafarAkhondali/acer-predator-turbo-and-rgb-keyboard-linux-module for setting 4zone keyboard rgb.

<p>Usage:<br>
Setting fan speeds<br>
*Cpu Fan(Left) echo fan-fan1-{fanspeed} /dev/acergaming<br>
*Gpu Fan(Right) echo fan-fan2-{fanspeed} /dev/acergaming<br>
Note: the fan speed should be multiples of 128. eg. 128 256 512 768<br>
Setting keyboard rgb as 4 zone (Note: In the stupidly original NitroSense app there is we cant set different zones to different colors when using the dynamic animations, but here we can do.)<br>
echo 4zone-{zone}-{red}-{green}-{blue}<br>
Setting keyboard rgb as dynamic:<br>
echo dyn-{mode}-{speed}-{brightness}-{direction}-{red}-{green}-{blue}<p/>

Some things can be bugprone, feel free to report issues from here.

<p>To-do: Implementing the turbo mode in NitroSense<br>
Making turbo button usable <br>
Making nitrosense button usable <br>
Making a dkms module <br>
Adding it to Aur <br>
Making a CTL(planning nitroctl) and writing a systemctl and openrc service to start this automatically and remember the settings every boot. </p>

NOTE: This driver is experimental and can be dangereous, i am taking no responsibility.
  
