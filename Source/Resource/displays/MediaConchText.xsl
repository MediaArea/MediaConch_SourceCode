<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi">
  <xsl:output encoding="UTF-8" method="text" version="1.0" indent="yes"/>
  <xsl:template match="/mc:MediaConch">
    <xsl:text>*********************&#xa;</xsl:text>
    <xsl:text>* MediaConch report *&#xa;</xsl:text>
    <xsl:text>*********************&#xa;</xsl:text>
    <xsl:if test="mc:name != '' or mc:description != ''">
      <xsl:if test="mc:name != ''">
        <xsl:value-of select="mc:name"/>
        <xsl:text>&#xa;</xsl:text>
      </xsl:if>
      <xsl:if test="mc:description != ''">
        <xsl:value-of select="mc:description"/>
        <xsl:text>&#xa;</xsl:text>
      </xsl:if>
      <xsl:text>&#xa;</xsl:text>
    </xsl:if>
    <xsl:for-each select="mc:media">
      <xsl:text>******************************************************************************&#xa;</xsl:text>
      <xsl:value-of select="@ref"/>
      <xsl:text>&#xa;</xsl:text>
      <xsl:for-each select="mc:implementationChecks">
        <xsl:text>*************************&#xa;</xsl:text>
        <xsl:text>* Implementation Checks *&#xa;</xsl:text>
        <xsl:text>*************************&#xa;</xsl:text>
        <xsl:if test="mc:name != '' or mc:description != ''">
          <xsl:text>&#xa;</xsl:text>
          <xsl:if test="mc:name != ''">
            <xsl:value-of select="mc:name"/>
            <xsl:text>&#xa;</xsl:text>
          </xsl:if>
          <xsl:if test="mc:description != ''">
            <xsl:value-of select="mc:description"/>
            <xsl:text>&#xa;</xsl:text>
          </xsl:if>
          <xsl:text>&#xa;</xsl:text>
        </xsl:if>
        <xsl:for-each select="mc:check">
          <xsl:text>------------------------------------------------------------------------------&#xa;</xsl:text>
          <xsl:value-of select="@icid"/><xsl:text>&#xa;</xsl:text>
          <xsl:if test="mc:context/@name != ''">
            <xsl:text>Context (name): </xsl:text>
            <xsl:value-of select="mc:context/@name"/>
            <xsl:text>&#xa;</xsl:text>
          </xsl:if>
          <xsl:for-each select="mc:test">
            <xsl:for-each select="mc:value">
              <xsl:if test="@name != ''">
                <xsl:value-of select="@name"/>
                <xsl:text>=</xsl:text>
              </xsl:if>
              <xsl:value-of select="."/>
              <xsl:text>, </xsl:text>
            </xsl:for-each>
            <xsl:text>Outcome: </xsl:text>
            <xsl:value-of select="@outcome"/>
            <xsl:text>&#xa;</xsl:text>
            <xsl:if test="@reason != ''">
              <xsl:text>Reason: </xsl:text>
              <xsl:value-of select="@reason"/>
              <xsl:text>&#xa;</xsl:text>
            </xsl:if>
          </xsl:for-each>
        </xsl:for-each>
      </xsl:for-each>
      <xsl:apply-templates select="mc:rule|mc:policy"/>
    </xsl:for-each>
    <xsl:text>******************************************************************************&#xa;</xsl:text>
  </xsl:template>
  <xsl:template match="mc:policy">
    <xsl:value-of select="substring('                    ',1,count(ancestor::*))"/>
    <xsl:text>* Policy Checks *&#xa;</xsl:text>
    <xsl:if test="@name != ''">
      <xsl:value-of select="substring('                    ',1,count(ancestor::*))"/>
      <xsl:text>Name: </xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>&#xa;</xsl:text>
    </xsl:if>
    <xsl:if test="mc:description != ''">
      <xsl:value-of select="substring('                    ',1,count(ancestor::*))"/>
      <xsl:text>Description: </xsl:text>
      <xsl:value-of select="mc:description"/>
      <xsl:text>&#xa;</xsl:text>
    </xsl:if>
    <xsl:if test="@type != ''">
      <xsl:value-of select="substring('                    ',1,count(ancestor::*))"/>
      <xsl:text>Type: </xsl:text>
      <xsl:value-of select="@type"/>
      <xsl:text>&#xa;</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring('                    ',1,count(ancestor::*))"/>
    <xsl:text>Outcome: </xsl:text>
    <xsl:value-of select="@outcome"/>
    <xsl:text>&#xa;</xsl:text>
    <xsl:if test="@rules_run != ''">
      <xsl:value-of select="substring('                    ',1,count(ancestor::*))"/>
      <xsl:text>Rules run: </xsl:text>
      <xsl:value-of select="@rules_run"/>
      <xsl:text>&#xa;</xsl:text>
    </xsl:if>
    <xsl:if test="@fail_count != ''">
      <xsl:value-of select="substring('                    ',1,count(ancestor::*))"/>
      <xsl:text>Fail count: </xsl:text>
      <xsl:value-of select="@fail_count"/>
      <xsl:text>&#xa;</xsl:text>
    </xsl:if>
    <xsl:if test="@pass_count != ''">
      <xsl:value-of select="substring('                    ',1,count(ancestor::*))"/>
      <xsl:text>Pass count: </xsl:text>
      <xsl:value-of select="@pass_count"/>  
      <xsl:text>&#xa;</xsl:text>
    </xsl:if>
    <xsl:apply-templates select="mc:rule|mc:policy"/>
  </xsl:template>
  <xsl:template match="mc:rule">
    <xsl:value-of select="substring('                    ',1,count(ancestor::*))"/>
    <xsl:value-of select="substring('------------------------------------------------------------------------------',1,80-count(ancestor::*))"/>
    <xsl:text>&#xa;</xsl:text>
    <xsl:if test="@xpath != ''">
      <xsl:value-of select="substring('                    ',1,count(ancestor::*))"/>
      <xsl:text>Xpath: </xsl:text>
      <xsl:value-of select="@xpath"/>
      <xsl:text>&#xa;</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring('                    ',1,count(ancestor::*))"/>
    <xsl:text>Outcome: </xsl:text>
    <xsl:value-of select="@outcome"/>
    <xsl:text>&#xa;</xsl:text>
    <xsl:if test="@actual != ''">
      <xsl:value-of select="substring('                    ',1,count(ancestor::*))"/>
      <xsl:text>Actual: </xsl:text>
      <xsl:value-of select="@actual"/>
      <xsl:text>&#xa;</xsl:text>
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>
