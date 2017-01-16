<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi">
  <xsl:output encoding="UTF-8" method="xml" version="1.0" indent="yes"/>
  <xsl:template match="report">
    <MediaConch>
      <xsl:attribute name="version">
        <xsl:text>0.2</xsl:text>
      </xsl:attribute>
      <media>
        <xsl:attribute name="ref">
          <xsl:value-of select="file_info/fullpath"/>
        </xsl:attribute>
        <xsl:for-each select="implementation_checker">
          <implementationChecks>
            <xsl:attribute name="checks_run">
              <xsl:value-of select="count(implementation_check/rule_result)"/>
            </xsl:attribute>
            <xsl:attribute name="fail_count">
              <xsl:value-of select="@totalErrors"/>
            </xsl:attribute>
            <name>
              <xsl:text>dpfmanager:</xsl:text>
              <xsl:value-of select="implementation_check/name"/>
            </name>
            <xsl:for-each select="implementation_check/rule_result">
              <check>
                <xsl:attribute name="icid">
                  <xsl:value-of select="ruleTest"/>
                </xsl:attribute>
                <xsl:attribute name="name">
                  <xsl:value-of select="message"/>
                </xsl:attribute>
                <xsl:attribute name="tests_run">1</xsl:attribute>
                <xsl:attribute name="fail_count">
                  <xsl:choose>
                    <xsl:when test="level='critical'">1</xsl:when>
                    <xsl:otherwise>0</xsl:otherwise>
                  </xsl:choose>
                </xsl:attribute>
                <xsl:attribute name="pass_count">0</xsl:attribute>
                <context>
                  <xsl:attribute name="name">context</xsl:attribute>
                  <xsl:value-of select="context"/>
                </context>
                <context>
                  <xsl:attribute name="name">ruleId</xsl:attribute>
                  <xsl:value-of select="ruleId"/>
                </context>
                <context>
                  <xsl:attribute name="name">ruleValue</xsl:attribute>
                  <xsl:value-of select="ruleValue"/>
                </context>
                <context>
                  <xsl:attribute name="name">iso_reference</xsl:attribute>
                  <xsl:value-of select="iso_reference"/>
                </context>
                <test>
                  <xsl:attribute name="outcome">
                    <xsl:choose>
                      <xsl:when test="level='critical'">fail</xsl:when>
                      <xsl:otherwise>pass</xsl:otherwise>
                    </xsl:choose>
                  </xsl:attribute>
                  <xsl:attribute name="reason">
                    <xsl:value-of select="ruleValue"/>
                  </xsl:attribute>
                  <xsl:if test="location">
                    <value>
                      <xsl:attribute name="context">
                        <xsl:value-of select="location"/>
                      </xsl:attribute>
                    </value>
                  </xsl:if>
                </test>
              </check>
            </xsl:for-each>
          </implementationChecks>
        </xsl:for-each>
      </media>
    </MediaConch>
  </xsl:template>
</xsl:stylesheet>
