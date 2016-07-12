<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:aliasxsl="my:namespace" version="1.0" exclude-result-prefixes="aliasxsl">
  <xsl:output encoding="UTF-8" method="xml" version="1.0" indent="yes"/>
  <xsl:template match="/">
    <aliasxsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="https://mediaarea.net/mediaconch" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:ma="https://mediaarea.net/mediaarea" xmlns:mi="https://mediaarea.net/mediainfo" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:exsl="http://exslt.org/common" version="1.0" extension-element-prefixes="exsl xsi ma mc">
      <aliasxsl:output encoding="UTF-8" method="xml" version="1.0" indent="yes"/>
      <aliasxsl:template match="ma:MediaArea">
        <MediaConch>
          <aliasxsl:attribute name="version">
            <aliasxsl:text>0.3</aliasxsl:text>
          </aliasxsl:attribute>
          <aliasxsl:for-each select="ma:media">
            <media>
              <aliasxsl:attribute name="ref">
                <aliasxsl:value-of select="./@ref"/>
              </aliasxsl:attribute>
              <xsl:for-each select="policy">
                <xsl:call-template name="policycheck">
                  <xsl:with-param name="policy" select="."/>
                </xsl:call-template>
              </xsl:for-each>
              <xsl:for-each select="test">
                <xsl:call-template name="testcheck">
                  <xsl:with-param name="test" select="."/>
                </xsl:call-template>
              </xsl:for-each>
            </media>
          </aliasxsl:for-each>
        </MediaConch>
      </aliasxsl:template>
      <aliasxsl:template name="test">
        <aliasxsl:param name="xpath"/>
        <aliasxsl:param name="outcome"/>
        <aliasxsl:param name="actual"/>
        <test>
          <aliasxsl:if test="../@type">
            <aliasxsl:attribute name="tracktype">
              <aliasxsl:value-of select="../@type"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:if test="../@typeorder">
            <aliasxsl:attribute name="tracktypeorder">
              <aliasxsl:value-of select="../@typeorder"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:if test="../mi:ID">
            <aliasxsl:attribute name="trackid">
              <aliasxsl:value-of select="../mi:ID"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:attribute name="xpath">
            <aliasxsl:value-of select="$xpath"/>
          </aliasxsl:attribute>
          <aliasxsl:if test="$outcome='fail'">
            <aliasxsl:attribute name="actual">
              <aliasxsl:value-of select="$actual"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:attribute name="outcome">
            <aliasxsl:value-of select="$outcome"/>
          </aliasxsl:attribute>
        </test>
      </aliasxsl:template>
      <aliasxsl:template name="check">
        <aliasxsl:param name="name"/>
        <aliasxsl:param name="description"/>
        <aliasxsl:param name="type"/>
        <aliasxsl:param name="context"/>
        <aliasxsl:param name="value"/>
        <aliasxsl:param name="test"/>
        <aliasxsl:param name="testresults"/>
        <aliasxsl:param name="morechecks"/>
        <policy>
          <aliasxsl:attribute name="name">
            <aliasxsl:value-of select="$name"/>
          </aliasxsl:attribute>
          <aliasxsl:attribute name="type">
            <aliasxsl:value-of select="$type"/>
          </aliasxsl:attribute>
          <aliasxsl:variable name="tests_run">
            <aliasxsl:value-of select="count(ealiasxsl:node-set($testresults)/mc:test) + count(ealiasxsl:node-set($morechecks)/mc:check)"/>
          </aliasxsl:variable>
          <aliasxsl:variable name="fail_count">
            <aliasxsl:value-of select="count(ealiasxsl:node-set($testresults)/mc:test[@outcome='fail']) + count(ealiasxsl:node-set($morechecks)/mc:check[@outcome='fail'])"/>
          </aliasxsl:variable>
          <aliasxsl:variable name="pass_count">
            <aliasxsl:value-of select="count(ealiasxsl:node-set($testresults)/mc:test[@outcome='pass']) + count(ealiasxsl:node-set($morechecks)/mc:check[@outcome='pass'])"/>
          </aliasxsl:variable>
          <aliasxsl:attribute name="tests_run">
            <aliasxsl:value-of select="$tests_run"/>
          </aliasxsl:attribute>
          <aliasxsl:attribute name="fail_count">
            <aliasxsl:value-of select="$fail_count"/>
          </aliasxsl:attribute>
          <aliasxsl:attribute name="pass_count">
            <aliasxsl:value-of select="$pass_count"/>
          </aliasxsl:attribute>
          <aliasxsl:attribute name="outcome">
            <aliasxsl:choose>
              <aliasxsl:when test="$type = 'or' and $pass_count &gt;= 1">pass</aliasxsl:when>
              <aliasxsl:when test="$type = 'and' and $fail_count = '0'">pass</aliasxsl:when>
              <aliasxsl:otherwise>fail</aliasxsl:otherwise>
            </aliasxsl:choose>
          </aliasxsl:attribute>
          <aliasxsl:if test="$description">
            <description>
              <aliasxsl:value-of select="$description"/>
            </description>
          </aliasxsl:if>
          <aliasxsl:copy-of select="$context"/>
          <aliasxsl:copy-of select="$testresults"/>
          <aliasxsl:copy-of select="$morechecks"/>
          <aliasxsl:choose>
            <aliasxsl:when test="count(ealiasxsl:node-set($testresults)/mc:test)+count(ealiasxsl:node-set($morechecks)/mc:check)='0'">
              <test outcome="N/A"/>
            </aliasxsl:when>
          </aliasxsl:choose>
        </policy>
      </aliasxsl:template>
    </aliasxsl:stylesheet>
  </xsl:template>
  <xsl:template name="policycheck">
    <xsl:for-each select=".">
      <aliasxsl:call-template name="check">
        <aliasxsl:with-param name="name">
          <xsl:value-of select="@name"/>
        </aliasxsl:with-param>
        <aliasxsl:with-param name="description">
          <xsl:value-of select="description"/>
        </aliasxsl:with-param>
        <aliasxsl:with-param name="type">
          <xsl:value-of select="@type"/>
        </aliasxsl:with-param>
        <aliasxsl:with-param name="testresults">
          <xsl:for-each select="test">
            <xsl:call-template name="testcheck">
              <xsl:with-param name="test" select="."/>
            </xsl:call-template>
          </xsl:for-each>
        </aliasxsl:with-param>
        <aliasxsl:with-param name="morechecks">
          <xsl:for-each select="policy">
            <xsl:call-template name="policycheck">
              <xsl:with-param name="policy" select="."/>
            </xsl:call-template>
          </xsl:for-each>
        </aliasxsl:with-param>
      </aliasxsl:call-template>
    </xsl:for-each>
  </xsl:template>
  <xsl:template name="testcheck">
    <xsl:param name="test"/>
    <xsl:variable name="equationfull">
      <xsl:text>mi:MediaInfo/mi:track[@type='</xsl:text>
      <xsl:value-of select="@tracktype"/>
      <xsl:text>'][</xsl:text>
      <xsl:value-of select="@occurrence"/>
      <xsl:text>]/mi:</xsl:text>
      <xsl:value-of select="@value"/>
      <xsl:value-of select="@operator"/>
      <xsl:value-of select="."/>
    </xsl:variable>
    <xsl:variable name="equationbase">
      <xsl:text>mi:MediaInfo/mi:track[@type='</xsl:text>
      <xsl:value-of select="@tracktype"/>
      <xsl:text>'][</xsl:text>
      <xsl:value-of select="@occurrence"/>
      <xsl:text>]/mi:</xsl:text>
      <xsl:value-of select="@value"/>
    </xsl:variable>
    <aliasxsl:call-template name="test">
      <aliasxsl:with-param name="xpath">
        <xsl:value-of select="$equationfull"/>
      </aliasxsl:with-param>
      <aliasxsl:with-param name="actual">
        <xsl:attribute name="select">
          <xsl:value-of select="$equationbase"/>
        </xsl:attribute>
      </aliasxsl:with-param>
      <aliasxsl:with-param name="outcome">
        <aliasxsl:choose>
          <aliasxsl:when>
            <xsl:attribute name="test">
              <xsl:value-of select="$equationfull"/>
            </xsl:attribute>
            <xsl:text>pass</xsl:text>
          </aliasxsl:when>
          <aliasxsl:otherwise>fail</aliasxsl:otherwise>
        </aliasxsl:choose>
      </aliasxsl:with-param>
    </aliasxsl:call-template>
  </xsl:template>
</xsl:stylesheet>
