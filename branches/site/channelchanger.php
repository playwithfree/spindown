<?php $title = "Channelchanger";
  include("header.html"); ?>

<h1>Channelchanger</h1>

<p>Channelchanger is a bash script I've written to change the channel of my tv device under Linux, it should work with every ivtv based tv tuner cards on Linux. It shows you a pretty menu using dialog where you can then chose your channel or just chose the next and the previous channel. The channels, frequencies and the video device to use are read from a ini configuration file.</p>

<p>I've tested channelchanger with an Hauppauge WinTV PVR-150MCE and an Hauppauge WinTV PVRUSB2. And they both worked the same.</p>

<h2>Dependencies</h2>
<p>To use Channelchanger you will need at least the following:</p>

<ul>
    <li>An ivtv based video card</li>
    <li>The <a href="http://ivtvdriver.org/index.php/Ivtv-tune">ivtv-tune</a> program</li>
    <li><a href="http://invisible-island.net/dialog/dialog.html">dialog</a></li>
    <li> And bash</li>
</ul>

<h2>Usage</h2>
<p>I watch tv by piping the output from the video device the xine and then I open channelchanger to change the stations. I use the following command:</p>

<pre>     cat /dev/video0 | xine -D stdin://mpeg2 &amp;&gt; /dev/null &amp; channelchanger</pre>

<p>(I use yakuake as my terminal emulator, so it's really easy for me to access the program.)</p>

<p>You can specify another configuration file if you would like:</p>

<pre>    channelchanger ~/.channels.conf.2</pre>

<h2>Configuration file </h2>
<p>Channelchanger uses an ini file to add to specify the channels, by default this file is in your home directory and has the name .channels.conf </p>

<p>It should look something like this</p>

<pre>    ; General settings
    [general]
    ; the video device we are going to use
    devi=/dev/video0

    ; Now for the channels:
    ; Every channel section represents a channel for each
    ; channel there has to be a name and frequency parameter
    [channel]
    name=Nederland 1
    freq=479.25

    [channel]
    name=Nederland 2
    freq=503.25
</pre>

<p>You can add as many channels if you like if devi is not set /dev/video0 is used by default.</p>

<h2>Contact</h2>
<p>For support <a href="http://projects.dimis-site.be/forum/">please see the forums</a>.</p>


<h2>Screenshots</h2>
<p class="center"><a href="images/channelchanger01.png"><img src="images/channelchanger01_t.png" alt="channelchanger" /></a></p>

<h2>Download</h2>
<p>You can download channelchanger from here:</p>

<p><a href="files/channelchanger-v0.1.tar.gz">Channelchanger v0.1</a></p>

<?php $updated = "17/03/2008";
  include("footer.html"); ?>