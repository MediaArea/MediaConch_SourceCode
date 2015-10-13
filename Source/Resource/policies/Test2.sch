<?xml version="1.0" encoding="UTF-8"?>
<sch:schema xmlns:sch="http://www.ascc.net/xml/schematron">
  
  <sch:ns prefix="ma" uri="https://mediaarea.net/mediaarea" /> 

    <sch:pattern name="Each file has unique ID">
        <sch:rule context="/ma:MediaArea/ma:media/ma:MediaInfo">
            <sch:assert test="track[@type='General']/UniqueID">Unique ID must exist.</sch:assert>
        </sch:rule>
    </sch:pattern>
    
    <sch:pattern name="General Format must be Matroska">
        <sch:rule context="/ma:MediaArea/ma:media/ma:MediaInfo">
            <sch:assert test="track[@type='General']/Format = 'Matroska'">General Format must be Matroska.</sch:assert>
        </sch:rule>
    </sch:pattern>
    
    <sch:pattern name="Extension must be mkv">
        <sch:rule context="/ma:MediaArea/ma:media/ma:MediaInfo">
            <sch:assert test="track[@type='General']/FileExtension = 'mkv'">File extension must be mkv.</sch:assert>
        </sch:rule>
    </sch:pattern>
    
    <sch:pattern name="Duration must be 40 ms">
        <sch:rule context="/ma:MediaArea/ma:media/ma:MediaInfo">
            <sch:assert test="track[@type='General']/Duration = 40">Duration must be 40 ms.</sch:assert>
        </sch:rule>
    </sch:pattern>
    
    <sch:pattern name="Frame rate must be 25">
        <sch:rule context="/ma:MediaArea/ma:media/ma:MediaInfo">
            <sch:assert test="track[@type='General']/FrameRate = 25">Frame rate must be 25.</sch:assert>
        </sch:rule>
    </sch:pattern>
    
    <sch:pattern name="File has a video stream.">
        <sch:rule context="/ma:MediaArea/ma:media/ma:MediaInfo">
            <sch:assert test="track[@type='Video']">Video stream must exist.</sch:assert>
        </sch:rule>
    </sch:pattern>
    
    <sch:pattern name="Video file has unique ID">
        <sch:rule context="/ma:MediaArea/ma:media/ma:MediaInfo">
            <sch:assert test="track[@type='Video']/UniqueID">Video track Unique ID must exist.</sch:assert>
        </sch:rule>
    </sch:pattern>
    
    <sch:pattern name="Video Format">
        <sch:rule context="/ma:MediaArea/ma:media/ma:MediaInfo">
            <sch:assert test="track[@type='Video']/Format = 'RGB'">Video Format must equal RGB.</sch:assert>
        </sch:rule>
    </sch:pattern>
    
    <sch:pattern name="DisplayAspectRatio must be 1">
        <sch:rule context="/ma:MediaArea/ma:media/ma:MediaInfo">
            <sch:assert test="track[@type='Video']/DisplayAspectRatio = 1">DisplayAspectRatio must be 1.</sch:assert>
        </sch:rule>
    </sch:pattern>
    
</sch:schema>