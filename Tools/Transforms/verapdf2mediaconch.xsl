<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi">
  <xsl:output encoding="UTF-8" method="xml" version="1.0" indent="yes"/>
  <xsl:template match="report">
    <MediaConch>
      <xsl:attribute name="version">
        <xsl:text>0.2</xsl:text>
      </xsl:attribute>
      <xsl:for-each select="jobs/job">
        <media>
          <xsl:attribute name="ref">
            <xsl:value-of select="item/name"/>
          </xsl:attribute>
          <xsl:for-each select="validationReport">
            <implementationChecks>
              <xsl:attribute name="checks_run">
                <xsl:value-of select="details/@passedRules+details/@failedRules"/>
              </xsl:attribute>
              <xsl:attribute name="fail_count">
                <xsl:value-of select="details/@failedRules"/>
              </xsl:attribute>
              <xsl:attribute name="pass_count">
                <xsl:value-of select="details/@passedRules"/>
              </xsl:attribute>
              <xsl:if test="@profileName">
                <name>
                  <xsl:value-of select="@profileName"/>
                </name>
              </xsl:if>
              <xsl:if test="statement">
                <description>
                  <xsl:value-of select="statement"/>
                </description>
              </xsl:if>
              <xsl:for-each select="details/rule">
                <check>
                  <xsl:attribute name="icid">
                    <xsl:value-of select="@specification"/>
                    <xsl:text>/</xsl:text>
                    <xsl:value-of select="@clause"/>
                    <xsl:text>(</xsl:text>
                    <xsl:value-of select="@testNumber"/>
                    <xsl:text>)</xsl:text>
                  </xsl:attribute>
                  <xsl:attribute name="name">
                    <xsl:value-of select="test"/>
                  </xsl:attribute>
                  <xsl:attribute name="version">
                    <xsl:value-of select="//report/buildInformation/releaseDetails[@id='core']/@version"/>
                  </xsl:attribute>
                  <xsl:if test="@failedChecks and @passedChecks">
                    <xsl:attribute name="tests_run">
                      <xsl:value-of select="@passedChecks + @failedChecks"/>
                    </xsl:attribute>
                  </xsl:if>
                  <xsl:if test="@failedChecks">
                    <xsl:attribute name="fail_count">
                      <xsl:value-of select="@failedChecks"/>
                    </xsl:attribute>
                  </xsl:if>
                  <xsl:if test="@passedChecks">
                    <xsl:attribute name="pass_count">
                      <xsl:value-of select="@passedChecks"/>
                    </xsl:attribute>
                  </xsl:if>
                  <xsl:if test="@specification">
                    <context>
                      <xsl:attribute name="name">specification</xsl:attribute>
                      <xsl:value-of select="@specification"/>
                    </context>
                  </xsl:if>
                  <xsl:if test="@clause">
                    <context>
                      <xsl:attribute name="name">clause</xsl:attribute>
                      <xsl:value-of select="@clause"/>
                    </context>
                  </xsl:if>
                  <xsl:if test="@testNumber">
                    <context>
                      <xsl:attribute name="name">testNumber</xsl:attribute>
                      <xsl:value-of select="@testNumber"/>
                    </context>
                  </xsl:if>
                  <xsl:if test="description">
                    <context>
                      <xsl:attribute name="name">description</xsl:attribute>
                      <xsl:value-of select="description"/>
                    </context>
                  </xsl:if>
                  <xsl:if test="object">
                    <context>
                      <xsl:attribute name="name">object</xsl:attribute>
                      <xsl:value-of select="object"/>
                    </context>
                  </xsl:if>
                  <xsl:for-each select="check">
                    <test>
                      <xsl:attribute name="outcome">
                        <xsl:choose>
                          <xsl:when test="@status='failed'">fail</xsl:when>
                          <xsl:otherwise>pass</xsl:otherwise>
                        </xsl:choose>
                      </xsl:attribute>
                      <value>
                        <xsl:attribute name="context">
                          <xsl:value-of select="context"/>
                        </xsl:attribute>
                      </value>
                    </test>
                  </xsl:for-each>
                </check>
              </xsl:for-each>
            </implementationChecks>
          </xsl:for-each>
        </media>
      </xsl:for-each>
    </MediaConch>
  </xsl:template>
</xsl:stylesheet>
