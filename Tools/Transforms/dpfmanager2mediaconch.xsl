<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi">
  <xsl:output encoding="UTF-8" method="xml" version="1.0" indent="yes"/>
  <xsl:template match="globalreport">
    <MediaConch>
      <xsl:attribute name="version">
        <xsl:text>0.2</xsl:text>
      </xsl:attribute>
      <xsl:for-each select="individualreports/report">
        <media>
          <xsl:attribute name="ref">
            <xsl:value-of select="file_info/fullpath"/>
          </xsl:attribute>
          <xsl:for-each select="implementation_checker/*[result]">
            <implementationChecks>
              <xsl:attribute name="checks_run">
                <xsl:value-of select="count(result)"/>
              </xsl:attribute>
              <xsl:attribute name="fail_count">
                <xsl:value-of select="count(result[level='critical'])"/>
              </xsl:attribute>
              <xsl:attribute name="pass_count">
                <xsl:value-of select="count(result[level!='critical'])"/>
              </xsl:attribute>
              <name>
                <xsl:text>dpfmanager:</xsl:text>
                <xsl:value-of select="name(.)"/>
              </name>
              <xsl:for-each select="result">
                <check>
                  <xsl:attribute name="icid">n/a</xsl:attribute>
                  <xsl:attribute name="version">0</xsl:attribute>
                  <xsl:attribute name="tests_run">1</xsl:attribute>
                  <xsl:attribute name="fail_count">
                    <xsl:choose>
                      <xsl:when test="level='critical'">1</xsl:when>
                      <xsl:otherwise>0</xsl:otherwise>
                    </xsl:choose>
                  </xsl:attribute>
                  <xsl:attribute name="pass_count">0</xsl:attribute>
                  <test>
                    <xsl:attribute name="outcome">
                      <xsl:choose>
                        <xsl:when test="level='critical'">fail</xsl:when>
                        <xsl:otherwise>pass</xsl:otherwise>
                      </xsl:choose>
                    </xsl:attribute>
                    <xsl:attribute name="reason">
                      <xsl:value-of select="msg"/>
                    </xsl:attribute>
                  </test>
                </check>
              </xsl:for-each>
            </implementationChecks>
          </xsl:for-each>
        </media>
      </xsl:for-each>
    </MediaConch>
  </xsl:template>
</xsl:stylesheet>
