<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="https://mediaarea.net/mediaconch" xmlns:ma="https://mediaarea.net/mediaarea" xmlns:mi="https://mediaarea.net/mediainfo" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi ma">
  <xsl:output encoding="UTF-8" method="xml" version="1.0" indent="yes"/>
  <xsl:template match="ma:MediaArea">
    <MediaConch>
      <xsl:attribute name="version">
        <xsl:text>0.1</xsl:text>
      </xsl:attribute>
      <xsl:for-each select="ma:media">
        <media>
          <xsl:attribute name="ref">
            <xsl:value-of select="./@ref"/>
          </xsl:attribute>
          <policyChecks>
            <name>CAVPP Access Video Files</name>
            <description>Based upon documentation at http://calpreservation.org/projects/audiovisual-preservation/ http://calpreservation.org/wp-content/uploads/2014/12/CAVPP-file-specs-2014.11.20.pdf. Generate a comforming sample via: `ffmpeg -f lavfi -i mandelbrot=s=720x540:r=29.97 -f lavfi -i aevalsrc="sin(10*2*PI*t)*sin(880*2*PI*t):c=stereo:s=48000" -t 1 -c:v libx264 -b:v 3M -vf setdar=4/3 -c:a aac -b:a 163840  cavpp_access_sample.mp4`. Note that this example file actually fails the audio bitrate test. AAC encoders do not generally encode at constant bitrate, so special encoders are required to comply with this specification.</description>
            <check>
              <xsl:attribute name="name">mp4 wrapper</xsl:attribute>
              <context>
                <xsl:attribute name="field">Format</xsl:attribute>
                <xsl:attribute name="value">MPEG-4</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='General'][*]/mi:Format">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='General'][*]/mi:Format">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">MPEG-4</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">avc1 video</xsl:attribute>
              <context>
                <xsl:attribute name="field">CodecID</xsl:attribute>
                <xsl:attribute name="value">avc1</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][*]/mi:CodecID">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][*]/mi:CodecID">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">avc1</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">Minimum Video Bitrate (3.5Mbps with kilo=1024 is 3670016 bits)</xsl:attribute>
              <context>
                <xsl:attribute name="field">BitRate</xsl:attribute>
                <xsl:attribute name="value">3670016</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][*]/mi:BitRate">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][*]/mi:BitRate">
                    <xsl:call-template name="is_greater_or_equal_than">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">3670016</xsl:with-param>
                      <xsl:with-param name="field">BitRate</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">Frame Rate 29.970</xsl:attribute>
              <context>
                <xsl:attribute name="field">FrameRate</xsl:attribute>
                <xsl:attribute name="value">29.970</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][*]/mi:FrameRate">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][*]/mi:FrameRate">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">29.970</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">Aspect Ratio 4/3</xsl:attribute>
              <context>
                <xsl:attribute name="field">DisplayAspectRatio</xsl:attribute>
                <xsl:attribute name="value">1.333</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][*]/mi:DisplayAspectRatio">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][*]/mi:DisplayAspectRatio">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">1.333</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">Is progressive</xsl:attribute>
              <context>
                <xsl:attribute name="field">ScanType</xsl:attribute>
                <xsl:attribute name="value">Progressive</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][*]/mi:ScanType">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][*]/mi:ScanType">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">Progressive</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">Audio is 48kHz</xsl:attribute>
              <context>
                <xsl:attribute name="field">SamplingRate</xsl:attribute>
                <xsl:attribute name="value">48000</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Audio'][*]/mi:SamplingRate">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Audio'][*]/mi:SamplingRate">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">48000</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">Audio is stereo</xsl:attribute>
              <context>
                <xsl:attribute name="field">Channels</xsl:attribute>
                <xsl:attribute name="value">2</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Audio'][*]/mi:Channels">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Audio'][*]/mi:Channels">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">2</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">Audio is at least 157 Kb (157Kb = 160768 bits)</xsl:attribute>
              <context>
                <xsl:attribute name="field">BitRate</xsl:attribute>
                <xsl:attribute name="value">160768</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Audio'][*]/mi:BitRate">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Audio'][*]/mi:BitRate">
                    <xsl:call-template name="is_greater_or_equal_than">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">160768</xsl:with-param>
                      <xsl:with-param name="field">BitRate</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">Maximum Video Bitrate (4 Mbps with kilo=1024 is 4194304 bits)</xsl:attribute>
              <context>
                <xsl:attribute name="field">BitRate</xsl:attribute>
                <xsl:attribute name="value">4194304</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][*]/mi:BitRate">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][*]/mi:BitRate">
                    <xsl:call-template name="is_less_or_equal_than">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">4194304</xsl:with-param>
                      <xsl:with-param name="field">BitRate</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">YUV Colorspace</xsl:attribute>
              <context>
                <xsl:attribute name="field">ColorSpace</xsl:attribute>
                <xsl:attribute name="value">YUV</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][*]/mi:ColorSpace">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][*]/mi:ColorSpace">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">YUV</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">Video is 8 bit</xsl:attribute>
              <context>
                <xsl:attribute name="field">BitDepth</xsl:attribute>
                <xsl:attribute name="value">8</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][*]/mi:BitDepth">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][*]/mi:BitDepth">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">8</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">Audio is AAC</xsl:attribute>
              <context>
                <xsl:attribute name="field">Format</xsl:attribute>
                <xsl:attribute name="value">AAC</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Audio'][*]/mi:Format">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Audio'][*]/mi:Format">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">AAC</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">Audio is at most 160 Kb (160Kb = 163840 bits)</xsl:attribute>
              <context>
                <xsl:attribute name="field">BitRate</xsl:attribute>
                <xsl:attribute name="value">163840</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Audio'][*]/mi:BitRate">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Audio'][*]/mi:BitRate">
                    <xsl:call-template name="is_less_or_equal_than">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">163840</xsl:with-param>
                      <xsl:with-param name="field">BitRate</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                  <test outcome="N/A"/>
                </xsl:otherwise>
              </xsl:choose>
            </check>
          </policyChecks>
        </media>
      </xsl:for-each>
    </MediaConch>
  </xsl:template>
  <xsl:template name="is_true">
    <xsl:param name="xpath"/>
    <xsl:element name="test">
      <xsl:if test="../@type">
        <xsl:attribute name="tracktype">
          <xsl:value-of select="../@type"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../@typeorder">
        <xsl:attribute name="tracktypeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
        <xsl:attribute name="trackid">
          <xsl:value-of select="../mi:ID"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:choose>
        <xsl:when test="$xpath">
          <xsl:attribute name="outcome">pass</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="outcome">fail</xsl:attribute>
          <xsl:attribute name="reason">is not true</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:element>
  </xsl:template>
  <xsl:template name="is_equal">
    <xsl:param name="xpath"/>
    <xsl:param name="value"/>
    <xsl:element name="test">
      <xsl:if test="../@type">
        <xsl:attribute name="tracktype">
          <xsl:value-of select="../@type"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../@typeorder">
        <xsl:attribute name="tracktypeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
        <xsl:attribute name="trackid">
          <xsl:value-of select="../mi:ID"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="actual">
        <xsl:value-of select="$xpath"/>
      </xsl:attribute>
      <xsl:choose>
        <xsl:when test="$xpath = $value">
          <xsl:attribute name="outcome">pass</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="outcome">fail</xsl:attribute>
          <xsl:attribute name="reason">is not equal</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:element>
  </xsl:template>
  <xsl:template name="is_not_equal">
    <xsl:param name="xpath"/>
    <xsl:param name="value"/>
    <xsl:element name="test">
      <xsl:if test="../@type">
        <xsl:attribute name="tracktype">
          <xsl:value-of select="../@type"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../@typeorder">
        <xsl:attribute name="tracktypeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
        <xsl:attribute name="trackid">
          <xsl:value-of select="../mi:ID"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="actual">
        <xsl:value-of select="$xpath"/>
      </xsl:attribute>
      <xsl:choose>
        <xsl:when test="$xpath != $value">
          <xsl:attribute name="outcome">pass</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="outcome">fail</xsl:attribute>
          <xsl:attribute name="reason">is equal</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:element>
  </xsl:template>
  <xsl:template name="is_greater_than">
    <xsl:param name="xpath"/>
    <xsl:param name="value"/>
    <xsl:element name="test">
      <xsl:if test="../@type">
        <xsl:attribute name="tracktype">
          <xsl:value-of select="../@type"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../@typeorder">
        <xsl:attribute name="tracktypeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
        <xsl:attribute name="trackid">
          <xsl:value-of select="../mi:ID"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="actual">
        <xsl:value-of select="$xpath"/>
      </xsl:attribute>
      <xsl:choose>
        <xsl:when test="$xpath &gt; $value">
          <xsl:attribute name="outcome">pass</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="outcome">fail</xsl:attribute>
          <xsl:attribute name="reason">is less than or equal</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:element>
  </xsl:template>
  <xsl:template name="is_less_than">
    <xsl:param name="xpath"/>
    <xsl:param name="value"/>
    <xsl:element name="test">
      <xsl:if test="../@type">
        <xsl:attribute name="tracktype">
          <xsl:value-of select="../@type"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../@typeorder">
        <xsl:attribute name="tracktypeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
        <xsl:attribute name="trackid">
          <xsl:value-of select="../mi:ID"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="actual">
        <xsl:value-of select="$xpath"/>
      </xsl:attribute>
      <xsl:choose>
        <xsl:when test="$xpath &lt; $value">
          <xsl:attribute name="outcome">pass</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="outcome">fail</xsl:attribute>
          <xsl:attribute name="reason">is greater than or equal</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:element>
  </xsl:template>
  <xsl:template name="is_greater_or_equal_than">
    <xsl:param name="xpath"/>
    <xsl:param name="value"/>
    <xsl:element name="test">
      <xsl:if test="../@type">
        <xsl:attribute name="tracktype">
          <xsl:value-of select="../@type"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../@typeorder">
        <xsl:attribute name="tracktypeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
        <xsl:attribute name="trackid">
          <xsl:value-of select="../mi:ID"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="actual">
        <xsl:value-of select="$xpath"/>
      </xsl:attribute>
      <xsl:choose>
        <xsl:when test="$xpath &gt;= $value">
          <xsl:attribute name="outcome">pass</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="outcome">fail</xsl:attribute>
          <xsl:attribute name="reason">is less than</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:element>
  </xsl:template>
  <xsl:template name="is_less_or_equal_than">
    <xsl:param name="xpath"/>
    <xsl:param name="value"/>
    <xsl:element name="test">
      <xsl:if test="../@type">
        <xsl:attribute name="tracktype">
          <xsl:value-of select="../@type"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../@typeorder">
        <xsl:attribute name="tracktypeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
        <xsl:attribute name="trackid">
          <xsl:value-of select="../mi:ID"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="actual">
        <xsl:value-of select="$xpath"/>
      </xsl:attribute>
      <xsl:choose>
        <xsl:when test="$xpath &lt;= $value">
          <xsl:attribute name="outcome">pass</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="outcome">fail</xsl:attribute>
          <xsl:attribute name="reason">is greater than</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:element>
  </xsl:template>
  <xsl:template name="exists">
    <xsl:param name="xpath"/>
    <xsl:element name="test">
      <xsl:if test="../@type">
        <xsl:attribute name="tracktype">
          <xsl:value-of select="../@type"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../@typeorder">
        <xsl:attribute name="tracktypeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
        <xsl:attribute name="trackid">
          <xsl:value-of select="../mi:ID"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="actual">
        <xsl:value-of select="$xpath"/>
      </xsl:attribute>
      <xsl:choose>
        <xsl:when test="string-length($xpath) != 0">
          <xsl:attribute name="outcome">pass</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="outcome">fail</xsl:attribute>
          <xsl:attribute name="reason">does not exist</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:element>
  </xsl:template>
  <xsl:template name="does_not_exist">
    <xsl:param name="xpath"/>
    <xsl:element name="test">
      <xsl:if test="../@type">
        <xsl:attribute name="tracktype">
          <xsl:value-of select="../@type"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../@typeorder">
        <xsl:attribute name="tracktypeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
        <xsl:attribute name="trackid">
          <xsl:value-of select="../mi:ID"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="actual">
        <xsl:value-of select="$xpath"/>
      </xsl:attribute>
      <xsl:choose>
        <xsl:when test="string-length($xpath) = '0'">
          <xsl:attribute name="outcome">pass</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="outcome">fail</xsl:attribute>
          <xsl:attribute name="reason">exists</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:element>
  </xsl:template>
  <xsl:template name="contains_string">
    <xsl:param name="xpath"/>
    <xsl:param name="value"/>
    <xsl:element name="test">
      <xsl:if test="../@type">
        <xsl:attribute name="tracktype">
          <xsl:value-of select="../@type"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../@typeorder">
        <xsl:attribute name="tracktypeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
        <xsl:attribute name="trackid">
          <xsl:value-of select="../mi:ID"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="actual">
        <xsl:value-of select="$xpath"/>
      </xsl:attribute>
      <xsl:choose>
        <xsl:when test="contains($xpath, $value)">
          <xsl:attribute name="outcome">pass</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="outcome">fail</xsl:attribute>
          <xsl:attribute name="reason">does not contain</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:element>
  </xsl:template>
</xsl:stylesheet>
