<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi">
  <xsl:output encoding="UTF-8" method="text" version="1.0" indent="yes"/>
  <xsl:template match="/mc:MediaConch">
    <xsl:text>filename</xsl:text>
    <xsl:text>,</xsl:text>
    <xsl:text>overall</xsl:text>
    <xsl:for-each select="mc:media[1]/mc:policy/mc:policy|mc:media[1]/mc:policy/mc:rule|mc:media[1]/mc:rule|mc:media[1]/mc:implementationChecks/mc:check">
      <xsl:text>,</xsl:text>
      <xsl:choose>
        <xsl:when test="local-name()='check'">
          <xsl:value-of select="@icid"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>"</xsl:text>
          <xsl:value-of select="@name"/>
          <xsl:text>"</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
    <xsl:for-each select="mc:media">
      <xsl:text>&#xa;</xsl:text>
      <xsl:call-template name="escapecommas">
        <xsl:with-param name="text" select="@ref"/>
      </xsl:call-template>
      <xsl:text>,</xsl:text>
      <xsl:value-of select="mc:policy/@outcome|mc:rule/@outcome"/>
      <xsl:for-each select="mc:policy/mc:policy|mc:policy/mc:rule|mc:rule">
        <xsl:text>,</xsl:text>
        <xsl:value-of select="@outcome"/>
      </xsl:for-each>
      <xsl:for-each select="mc:implementationChecks/mc:check">
        <xsl:text>,</xsl:text>
        <xsl:choose>
          <xsl:when test="@fail_count = '0'">pass</xsl:when>
          <xsl:when test="@fail_count > '0'">fail</xsl:when>
        </xsl:choose>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:template>
  <xsl:template name="escapecommas">
    <xsl:param name="text"/>
    <xsl:choose>
      <xsl:when test="$text = ''" >
        <xsl:value-of select="$text"/>
      </xsl:when>
      <xsl:when test="contains($text, ',')">
        <xsl:value-of select="substring-before($text,',')"/>
        <xsl:text>\,</xsl:text>
        <xsl:call-template name="escapecommas">
          <xsl:with-param name="text" select="substring-after($text,',')"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$text"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
</xsl:stylesheet>
