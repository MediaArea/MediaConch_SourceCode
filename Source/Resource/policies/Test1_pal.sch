<?xml version="1.0" encoding="UTF-8"?>
<sch:schema xmlns:sch="http://www.ascc.net/xml/schematron">
    
    <sch:pattern name="File should have PAL framesize">
        <sch:rule context="/Mediainfo/File">
            <sch:assert test="track[@type='Video']/Width = 720 and track[@type='Video']/Height = 576">Framesize must be 720 × 576.</sch:assert>
        </sch:rule>
    </sch:pattern>
    
    <sch:pattern name="File should have PAL framerate">
        <sch:rule context="/Mediainfo/File">
            <sch:assert test="track[@type='General']/FrameRate = 25">FrameRate must be 25.</sch:assert>
        </sch:rule>
    </sch:pattern>
    
</sch:schema>