<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="https://mediaarea.net/mediaconch" xmlns:ma="https://mediaarea.net/mediaarea" xmlns:mi="https://mediaarea.net/mediainfo" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi ma">
  <xsl:output encoding="UTF-8" method="xml" version="1.0" indent="yes"/>
  <xsl:template match="ma:MediaArea">
    <MediaConch>
      <xsl:attribute name="version">
        <xsl:text>0.1</xsl:text>
      </xsl:attribute>
      <policyChecks>
        <title>Test 1: Conflicting Tests (NTSC)</title>
        <description>This policy checks files for a framesize of 720 x 480 and a framerate of 29.970.</description>
        <xsl:for-each select="ma:media">
          <media>
            <xsl:attribute name="ref">
              <xsl:value-of select="./@ref"/>
            </xsl:attribute>
            <policy>
              <xsl:attribute name="title">Video Width must be 720.</xsl:attribute>
              <context>
                <xsl:attribute name="field">Width</xsl:attribute>
                <xsl:attribute name="value">720</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Width">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Width">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">720</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
              </xsl:choose>
            </policy>
            <policy>
              <xsl:attribute name="title">Video Height must be 480.</xsl:attribute>
              <context>
                <xsl:attribute name="field">Height</xsl:attribute>
                <xsl:attribute name="value">480</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Height">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Height">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">480</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
              </xsl:choose>
            </policy>
            <policy>
              <xsl:attribute name="title">General FrameRate must be 29.970.</xsl:attribute>
              <context>
                <xsl:attribute name="field">FrameRate</xsl:attribute>
                <xsl:attribute name="value">29.970</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='General'][1]/mi:FrameRate">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='General'][1]/mi:FrameRate">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">29.970</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
              </xsl:choose>
            </policy>
          </media>
        </xsl:for-each>
      </policyChecks>
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
      <xsl:if test="../@streamid">
        <xsl:attribute name="streamid">
          <xsl:value-of select="../@streamid"/>
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
      <xsl:if test="../@streamid">
        <xsl:attribute name="streamid">
          <xsl:value-of select="../@streamid"/>
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
      <xsl:if test="../@streamid">
        <xsl:attribute name="streamid">
          <xsl:value-of select="../@streamid"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="expected">
        <xsl:value-of select="$value"/>
      </xsl:attribute>
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
      <xsl:if test="../@streamid">
        <xsl:attribute name="streamid">
          <xsl:value-of select="../@streamid"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="expected">
        <xsl:value-of select="$value"/>
      </xsl:attribute>
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
      <xsl:if test="../@streamid">
        <xsl:attribute name="streamid">
          <xsl:value-of select="../@streamid"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="expected">
        <xsl:value-of select="$value"/>
      </xsl:attribute>
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
      <xsl:if test="../@streamid">
        <xsl:attribute name="streamid">
          <xsl:value-of select="../@streamid"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="expected">
        <xsl:value-of select="$value"/>
      </xsl:attribute>
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
      <xsl:if test="../@streamid">
        <xsl:attribute name="streamid">
          <xsl:value-of select="../@streamid"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="expected">
        <xsl:value-of select="$value"/>
      </xsl:attribute>
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
      <xsl:if test="../@streamid">
        <xsl:attribute name="streamid">
          <xsl:value-of select="../@streamid"/>
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
      <xsl:if test="../@streamid">
        <xsl:attribute name="streamid">
          <xsl:value-of select="../@streamid"/>
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
      <xsl:if test="../@streamid">
        <xsl:attribute name="streamid">
          <xsl:value-of select="../@streamid"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="expected">
        <xsl:value-of select="$value"/>
      </xsl:attribute>
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
