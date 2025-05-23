<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi">
  <xsl:output encoding="UTF-8" method="text" version="1.0" indent="yes"/>
  <xsl:template match="/mc:MediaConch">
    <xsl:for-each select="mc:media">
      <xsl:if test="position() != '1'">
        <xsl:text>&#xa;</xsl:text>
      </xsl:if>
      <xsl:choose>
        <xsl:when test="boolean(mc:rule/@outcome|mc:policy/@outcome)">
          <xsl:value-of select="mc:rule/@outcome|mc:policy/@outcome"/>
        </xsl:when>
        <xsl:when test="boolean(mc:implementationChecks/@fail_count)">
          <xsl:choose>
            <xsl:when test="sum(mc:implementationChecks/@pass_count) = '0' and sum(mc:implementationChecks/@info_count) = '0' and sum(mc:implementationChecks/@warn_count) = '0' and sum(mc:implementationChecks/@fail_count) = '0'">N/A</xsl:when>
            <xsl:when test="sum(mc:implementationChecks/@fail_count) > '0'">fail</xsl:when>
            <xsl:when test="sum(mc:implementationChecks/@warn_count) > '0'">warn</xsl:when>
            <xsl:when test="sum(mc:implementationChecks/@info_count) > '0'">info</xsl:when>
            <xsl:otherwise>pass</xsl:otherwise>
          </xsl:choose>
        </xsl:when>
      </xsl:choose>
      <xsl:text>! </xsl:text>
      <xsl:value-of select="@ref"/>
      <xsl:apply-templates select="mc:implementationChecks"/>
      <xsl:apply-templates select="mc:rule|mc:policy"/>
    </xsl:for-each>
  </xsl:template>
  <xsl:template match="mc:implementationChecks">
    <xsl:for-each select="mc:check[@fail_count > '0'] | mc:check[@warn_count > '0'] | mc:check[@info_count > '0']">
      <xsl:text>&#xa;</xsl:text>
      <xsl:text> -- </xsl:text>
      <xsl:value-of select="@icid"/>
      <xsl:for-each select="mc:test[@outcome!='pass']">
        <xsl:text>&#xa;</xsl:text>
        <xsl:text> --</xsl:text>
        <xsl:value-of select="substring('                            ',1,count(ancestor::*))"/>
        <xsl:text>[</xsl:text>
        <xsl:value-of select="@outcome"/>
        <xsl:if test="@reason">
          <xsl:text>:</xsl:text>
          <xsl:value-of select="@reason"/>
        </xsl:if>
        <xsl:text>]</xsl:text>
        <xsl:for-each select="mc:value">
          <xsl:text>&#xa;</xsl:text>
          <xsl:text> --</xsl:text>
          <xsl:value-of select="substring('                            ',1,count(ancestor::*))"/>
          <xsl:text>[</xsl:text>
          <xsl:value-of select="@context"/>
          <xsl:if test="text() != ''">
            <xsl:text>: </xsl:text>
            <xsl:value-of select="text()"/>
          </xsl:if>
          <xsl:if test="@offset">
            <xsl:text> at bytes offset of </xsl:text>
            <xsl:value-of select="@offset"/>
          </xsl:if>
          <xsl:text>]</xsl:text>
        </xsl:for-each>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:template>
  <xsl:template match="mc:policy|mc:rule">
    <xsl:if test="(name(..)!='policy' and @outcome!='pass') or (name(..)='policy' and ../@type='or' and ../@outcome!='pass' and @outcome=../@outcome) or (name(..)='policy' and ../@type!='or' and @outcome!='pass')">
      <xsl:text>&#xa;</xsl:text>
      <xsl:text> </xsl:text>
      <xsl:text>  --</xsl:text>
      <xsl:value-of select="substring('                            ',1,count(ancestor::*))"/>
      <xsl:text>[</xsl:text>
      <xsl:value-of select="@outcome"/>
      <xsl:text>:</xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>]</xsl:text>
      <xsl:apply-templates select="mc:policy|mc:rule"/>
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>
