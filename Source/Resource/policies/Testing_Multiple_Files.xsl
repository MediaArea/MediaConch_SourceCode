<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="https://mediaarea.net/mediaconch" xmlns:ma="https://mediaarea.net/mediaarea" xmlns:mi="https://mediaarea.net/mediainfo" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi ma">
  <xsl:output encoding="UTF-8" method="xml" version="1.0" indent="yes"/>
  <xsl:template match="ma:MediaArea">
    <MediaConch>
      <xsl:attribute name="version">
        <xsl:text>0.1</xsl:text>
      </xsl:attribute>
      <policyChecks>
        <name>Testing Multiple Files</name>
        <description>This policy checks all the files for conformance. Test2_2.mkv and Test2_3.mkv fail because they have been manipulated, but they fail in different ways.</description>
        <xsl:for-each select="ma:media">
          <media>
            <xsl:attribute name="ref">
              <xsl:value-of select="./@ref"/>
            </xsl:attribute>
            <check>
              <xsl:attribute name="name">General UniqueID must exist. </xsl:attribute>
              <context>
                <xsl:attribute name="field">UniqueID</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='General'][1]/mi:UniqueID">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='General'][1]/mi:UniqueID">
                    <xsl:call-template name="exists">
                      <xsl:with-param name="xpath" select="."/>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">General Format must be Matroska</xsl:attribute>
              <context>
                <xsl:attribute name="field">Format</xsl:attribute>
                <xsl:attribute name="value">Matroska</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='General'][1]/mi:Format">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='General'][1]/mi:Format">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">Matroska</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">General FileExtension must be mkv.</xsl:attribute>
              <context>
                <xsl:attribute name="value">track[@type='General']/FileExtension = 'mkv'</xsl:attribute>
              </context>
              <xsl:call-template name="is_true">
                <xsl:with-param name="xpath" select="track[@type='General']/FileExtension = 'mkv'"/>
              </xsl:call-template>
            </check>
            <check>
              <xsl:attribute name="name">General Duration must be 40 ms.</xsl:attribute>
              <context>
                <xsl:attribute name="field">Duration</xsl:attribute>
                <xsl:attribute name="value">40</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='General'][1]/mi:Duration">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='General'][1]/mi:Duration">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">40</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">General FrameRate must be 25</xsl:attribute>
              <context>
                <xsl:attribute name="field">FrameRate</xsl:attribute>
                <xsl:attribute name="value">25</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][1]/mi:FrameRate">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][1]/mi:FrameRate">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">25</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">Video stream must exist.</xsl:attribute>
              <context>
                <xsl:attribute name="value">track[@type='Video']</xsl:attribute>
              </context>
              <xsl:call-template name="is_true">
                <xsl:with-param name="xpath" select="track[@type='Video']"/>
              </xsl:call-template>
            </check>
            <check>
              <xsl:attribute name="name">Video Unique ID must exist.</xsl:attribute>
              <context>
                <xsl:attribute name="value">track[@type='Video']/UniqueID</xsl:attribute>
              </context>
              <xsl:call-template name="is_true">
                <xsl:with-param name="xpath" select="track[@type='Video']/UniqueID"/>
              </xsl:call-template>
            </check>
            <check>
              <xsl:attribute name="name">Video Format must equal RGB</xsl:attribute>
              <context>
                <xsl:attribute name="field">Format</xsl:attribute>
                <xsl:attribute name="value">RGB</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Format">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Format">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">RGB</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
              </xsl:choose>
            </check>
            <check>
              <xsl:attribute name="name">Video DisplayAspectRatio must be 1.</xsl:attribute>
              <context>
                <xsl:attribute name="field">DisplayAspectRatio</xsl:attribute>
                <xsl:attribute name="value">1.</xsl:attribute>
              </context>
              <xsl:choose>
                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][1]/mi:DisplayAspectRatio">
                  <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][1]/mi:DisplayAspectRatio">
                    <xsl:call-template name="is_equal">
                      <xsl:with-param name="xpath" select="."/>
                      <xsl:with-param name="value">1.</xsl:with-param>
                    </xsl:call-template>
                  </xsl:for-each>
                </xsl:when>
              </xsl:choose>
            </check>
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
      <xsl:if test="../@typeorder">
        <xsl:attribute name="typeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
          <xsl:attribute name="ID">
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
        <xsl:attribute name="typeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
          <xsl:attribute name="ID">
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
        <xsl:attribute name="typeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
          <xsl:attribute name="ID">
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
        <xsl:attribute name="typeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
          <xsl:attribute name="ID">
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
        <xsl:attribute name="typeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
          <xsl:attribute name="ID">
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
        <xsl:attribute name="typeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
          <xsl:attribute name="ID">
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
        <xsl:attribute name="typeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
          <xsl:attribute name="ID">
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
        <xsl:attribute name="typeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
          <xsl:attribute name="ID">
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
        <xsl:attribute name="typeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
          <xsl:attribute name="ID">
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
        <xsl:attribute name="typeorder">
          <xsl:value-of select="../@typeorder"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="../mi:ID">
          <xsl:attribute name="ID">
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
