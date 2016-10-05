<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="https://mediaarea.net/mediaconch" xmlns:v="http://www.verapdf.org/ValidationProfile" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="v xsi">
  <xsl:output encoding="UTF-8" method="xml" version="1.0" indent="yes"/>
  <xsl:template match="v:cliReport">
    <MediaConch>
      <xsl:attribute name="version">
        <xsl:text>0.2</xsl:text>
      </xsl:attribute>
      <media>
        <xsl:attribute name="ref">
          <xsl:value-of select="v:itemDetails/v:name"/>
        </xsl:attribute>
        <xsl:for-each select="v:validationResult">
          <implementationChecks>
            <xsl:attribute name="checks_run">
              <xsl:value-of select="@totalAssertions"/>
            </xsl:attribute>
            <xsl:attribute name="fail_count">
              <xsl:value-of select="count(v:assertions/v:assertion[@status='FAILED'])"/>
            </xsl:attribute>
            <xsl:attribute name="pass_count">0</xsl:attribute>
            <name>
              <xsl:text>verapdf2mediaconch: flavour </xsl:text>
              <xsl:value-of select="@flavour"/>
            </name>
            <xsl:for-each select="v:assertions/v:assertion">
              <check>
                <xsl:attribute name="icid">
                  <xsl:value-of select="@ordinal"/>
                </xsl:attribute>
                <xsl:attribute name="version">0</xsl:attribute>
                <xsl:attribute name="tests_run">1</xsl:attribute>
                <xsl:attribute name="fail_count">
                  <xsl:choose>
                    <xsl:when test="@status='FAILED'">1</xsl:when>
                    <xsl:otherwise>0</xsl:otherwise>
                  </xsl:choose>
                </xsl:attribute>
                <xsl:attribute name="pass_count">0</xsl:attribute>
                <context>
                  <xsl:attribute name="name">specification</xsl:attribute>
                  <xsl:value-of select="v:ruleId/@specification"/>
                </context>
                <context>
                  <xsl:attribute name="name">clause</xsl:attribute>
                  <xsl:value-of select="v:ruleId/@clause"/>
                </context>
                <context>
                  <xsl:attribute name="name">testNumber</xsl:attribute>
                  <xsl:value-of select="v:ruleId/@testNumber"/>
                </context>
                <test>
                  <xsl:attribute name="outcome">
                    <xsl:choose>
                      <xsl:when test="../@status='FAILED'">fail</xsl:when>
                      <xsl:otherwise>pass</xsl:otherwise>
                    </xsl:choose>
                  </xsl:attribute>
                  <value>
                    <xsl:attribute name="name">
                      <xsl:value-of select="v:location/v:level"/>
                    </xsl:attribute>
                    <xsl:attribute name="context">
                      <xsl:value-of select="v:location/v:context"/>
                    </xsl:attribute>
                    <xsl:value-of select="v:message"/>
                  </value>
                </test>
              </check>
            </xsl:for-each>
          </implementationChecks>
        </xsl:for-each>
      </media>
    </MediaConch>
  </xsl:template>
</xsl:stylesheet>
