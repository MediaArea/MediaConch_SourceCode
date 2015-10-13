<?xml version="1.0" encoding="UTF-8"?>
<sch:schema xmlns:sch="http://www.ascc.net/xml/schematron">
  
  <sch:ns prefix="ma" uri="https://mediaarea.net/mediaarea" /> 

    <sch:pattern name="File should have NTSC framesize">
        <sch:rule context="/ma:MediaArea/ma:media/ma:MediaInfo">
            <sch:assert test="track[@type='Video']/Width = 720 and track[@type='Video']/Height = 480">Framesize must be 720 × 480.</sch:assert>
        </sch:rule>
    </sch:pattern>

    <sch:pattern name="File should have NTSC framerate">
        <sch:rule context="/ma:MediaArea/ma:media/ma:MediaInfo">
            <sch:assert test="track[@type='General']/FrameRate = 29.970">FrameRate must be 29.970.</sch:assert>
        </sch:rule>
    </sch:pattern>
    
</sch:schema>