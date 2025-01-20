<?xml version="1.0"?>
<xsl:stylesheet xmlns:str="http://exslt.org/strings" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi">
  <xsl:output method="text" encoding="UTF-8" indent="yes"/>
  <xsl:variable name="indent" select="'&#xa;                                                                                                    '" />
  <xsl:template match="/mc:MediaConch">
    <xsl:variable name="level" select="'1'" />
    <xsl:text>{</xsl:text>
    <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
    <xsl:text>"MediaConch": {</xsl:text>
    <xsl:if test="@version">
      <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
      <xsl:text>"version": "</xsl:text>
      <xsl:value-of select="@version"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@verbosity">
      <xsl:if test="@version">
        <xsl:text>,</xsl:text>
    </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
      <xsl:text>"verbosity": </xsl:text>
      <xsl:value-of select="@verbosity"/>
    </xsl:if>
    <xsl:apply-templates select="mc:creatingApplication|mc:creatingLibrary">
      <xsl:with-param name="level" select="$level + 1"/>
      <xsl:with-param name="attributes" select="@version or @verbosity"/>
    </xsl:apply-templates>
    <xsl:apply-templates select="mc:implementationChecks|mc:policyChecks">
      <xsl:with-param name="level" select="$level + 1"/>
      <xsl:with-param name="attributes" select="@version or @verbosity"/>
    </xsl:apply-templates>
    <xsl:apply-templates select="mc:policy">
      <xsl:with-param name="level" select="$level + 1"/>
      <xsl:with-param name="attributes" select="@version or @verbosity"/>
    </xsl:apply-templates>
    <xsl:apply-templates select="mc:media">
      <xsl:with-param name="level" select="$level + 1"/>
      <xsl:with-param name="attributes" select="@version or @verbosity"/>
    </xsl:apply-templates>
    <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
    <xsl:text>}</xsl:text>
    <xsl:text>&#xa;}&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="mc:media">
    <xsl:param name="level"/>
    <xsl:param name="attributes"/>
    <xsl:if test="$attributes or preceding-sibling::*">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:if test="position() = 1">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>"media": [</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>{</xsl:text>
    <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
    <xsl:text>"ref": "</xsl:text>
    <xsl:value-of select="@ref"/>
    <xsl:text>"</xsl:text>
    <xsl:apply-templates select="mc:implementationChecks|mc:policyChecks">
      <xsl:with-param name="level" select="$level + 2"/>
      <xsl:with-param name="attributes" select="@ref"/>
    </xsl:apply-templates>
    <xsl:apply-templates select="mc:policy">
      <xsl:with-param name="level" select="$level + 2"/>
      <xsl:with-param name="attributes" select="@ref"/>
    </xsl:apply-templates>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>}</xsl:text>
    <xsl:if test="position() = last()">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>]</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="mc:implementationChecks|mc:policyChecks">
    <xsl:param name="level"/>
    <xsl:param name="attributes"/>
    <xsl:if test="$attributes or preceding-sibling::*">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:if test="position() = 1">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>"</xsl:text>
      <xsl:value-of select="name()"/>
      <xsl:text>": [</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>{</xsl:text>
    <xsl:if test="@pass_count">
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"pass_count": </xsl:text>
      <xsl:value-of select="@pass_count"/>
    </xsl:if>
    <xsl:if test="@info_count">
      <xsl:if test="@pass_count">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"info_count": </xsl:text>
      <xsl:value-of select="@info_count"/>
    </xsl:if>
    <xsl:if test="@warn_count">
      <xsl:if test="@pass_count or @info_count">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"warn_count": </xsl:text>
      <xsl:value-of select="@warn_count"/>
    </xsl:if>
    <xsl:if test="@fail_count">
      <xsl:if test="@pass_count or @info_count or @warn_count">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"fail_count": </xsl:text>
      <xsl:value-of select="@fail_count"/>
    </xsl:if>
    <xsl:if test="@checks_run">
      <xsl:if test="@pass_count or @info_count or @warn_count or @fail_count">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"checks_run": </xsl:text>
      <xsl:value-of select="@checks_run"/>
    </xsl:if>
    <xsl:if test="@version">
      <xsl:if test="@pass_count or @info_count or @warn_count or @fail_count or @checks_run">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"version": "</xsl:text>
      <xsl:value-of select="@version"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:apply-templates select="mc:name">
      <xsl:with-param name="level" select="$level + 2"/>
      <xsl:with-param name="attributes" select="@pass_count or @info_count or @warn_count or @fail_count or @checks_run or @version"/>
    </xsl:apply-templates>
    <xsl:apply-templates select="mc:description">
      <xsl:with-param name="level" select="$level + 2"/>
      <xsl:with-param name="attributes" select="@pass_count or @info_count or @warn_count or @fail_count or @checks_run or @version"/>
    </xsl:apply-templates>
    <xsl:apply-templates select="mc:check">
      <xsl:with-param name="level" select="$level + 2"/>
      <xsl:with-param name="attributes" select="@pass_count or @info_count or @warn_count or @fail_count or @checks_run or @version"/>
    </xsl:apply-templates>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>}</xsl:text>
    <xsl:if test="position() = last()">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>]</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="mc:name">
    <xsl:param name="level"/>
    <xsl:param name="attributes"/>
    <xsl:if test="$attributes or preceding-sibling::*">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
    <xsl:text>"name": "</xsl:text>
    <xsl:value-of select="node()"/>
    <xsl:text>"</xsl:text>
  </xsl:template>

  <xsl:template match="mc:description">
    <xsl:param name="level"/>
    <xsl:param name="attributes"/>
    <xsl:if test="$attributes or preceding-sibling::*">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
    <xsl:text>"description": "</xsl:text>
    <xsl:value-of select="str:replace(node(), '&#xa;', '\n')"/>
    <xsl:text>"</xsl:text>
  </xsl:template>

  <xsl:template match="mc:check">
    <xsl:param name="level"/>
    <xsl:param name="attributes"/>
    <xsl:if test="$attributes or preceding-sibling::*">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:if test="position() = 1">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>"checks": [</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>{</xsl:text>
    <xsl:if test="@icid">
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"icid": "</xsl:text>
      <xsl:value-of select="@icid"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@name">
      <xsl:if test="@icid">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"name": "</xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@version">
      <xsl:if test="@icid or @name">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"version": "</xsl:text>
      <xsl:value-of select="@version"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@pass_count">
      <xsl:if test="@icid or @name or @version">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"pass_count": </xsl:text>
      <xsl:value-of select="@pass_count"/>
    </xsl:if>
    <xsl:if test="@info_count">
      <xsl:if test="@icid or @name or @version or @pass_count">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"info_count": </xsl:text>
      <xsl:value-of select="@info_count"/>
    </xsl:if>
    <xsl:if test="@warn_count">
      <xsl:if test="@icid or @name or @version or @pass_count or @info_count">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"warn_count": </xsl:text>
      <xsl:value-of select="@warn_count"/>
    </xsl:if>
    <xsl:if test="@fail_count">
      <xsl:if test="@icid or @name or @version or @pass_count or @info_count or @warn_count">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"fail_count": </xsl:text>
      <xsl:value-of select="@fail_count"/>
    </xsl:if>
    <xsl:if test="@tests_run">
      <xsl:if test="@icid or @name or @version or @pass_count or @info_count or @warn_count or @fail_count">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"tests_run": </xsl:text>
      <xsl:value-of select="@tests_run"/>
    </xsl:if>
    <xsl:apply-templates select="mc:context">
      <xsl:with-param name="level" select="$level + 2"/>
      <xsl:with-param name="attributes" select="@icid or @name or @version or @pass_count or @info_count or @warn_count or @fail_count or @tests_run"/>
    </xsl:apply-templates>
    <xsl:apply-templates select="mc:test">
      <xsl:with-param name="level" select="$level + 2"/>
      <xsl:with-param name="attributes" select="@icid or @name or @version or @pass_count or @info_count or @warn_count or @fail_count or @tests_run"/>
    </xsl:apply-templates>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>}</xsl:text>
    <xsl:if test="position() = last()">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>]</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="mc:context">
    <xsl:param name="level"/>
    <xsl:param name="attributes"/>
    <xsl:if test="$attributes or preceding-sibling::*">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
    <xsl:text>"context": {</xsl:text>
    <xsl:if test="@name">
      <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
      <xsl:text>"name": "</xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>",</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>"value": "</xsl:text>
    <xsl:value-of select="node()"/>
    <xsl:text>"</xsl:text>
    <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
    <xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="mc:test">
    <xsl:param name="level"/>
    <xsl:param name="attributes"/>
    <xsl:if test="$attributes or preceding-sibling::*">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:if test="position() = 1">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>"tests": [</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>{</xsl:text>
    <xsl:if test="@tracktype">
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"tracktype": "</xsl:text>
      <xsl:value-of select="@tracktype"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@tracktypeorder">
      <xsl:if test="@tracktype">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"tracktypeorder": </xsl:text>
      <xsl:value-of select="@name"/>
    </xsl:if>
    <xsl:if test="@trackid">
      <xsl:if test="@tracktype or @tracktypeorder">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"trackid": "</xsl:text>
      <xsl:value-of select="@trackid"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@actual">
      <xsl:if test="@tracktype or @tracktypeorder or @trackid">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"actual": "</xsl:text>
      <xsl:value-of select="@actual"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@outcome">
      <xsl:if test="@tracktype or @tracktypeorder or @trackid or @actual">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"outcome": "</xsl:text>
      <xsl:value-of select="@outcome"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@reason">
      <xsl:if test="@tracktype or @tracktypeorder or @trackid or @actual or @outcome">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"reason": "</xsl:text>
      <xsl:value-of select="@reason"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:apply-templates select="mc:value">
      <xsl:with-param name="level" select="$level + 2"/>
      <xsl:with-param name="attributes" select="@tracktype or @tracktypeorder or @trackid or @actual or @outcome or @reason"/>
    </xsl:apply-templates>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>}</xsl:text>
    <xsl:if test="position() = last()">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>]</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="mc:value">
    <xsl:param name="level"/>
    <xsl:param name="attributes"/>
    <xsl:if test="$attributes or preceding-sibling::*">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:if test="position() = 1">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>"values": [</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>{</xsl:text>
    <xsl:if test="@name">
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"name": "</xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@formatid">
      <xsl:if test="@name">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"formatid": "</xsl:text>
      <xsl:value-of select="@formatid"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@offset">
      <xsl:if test="@name or @formatid">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"offset": "</xsl:text>
      <xsl:value-of select="@offset"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@context">
      <xsl:if test="@name or @formatid or @offset">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"context": "</xsl:text>
      <xsl:value-of select="@context"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@name or @formatid or @offset or @context">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
    <xsl:text>"value": "</xsl:text>
    <xsl:value-of select="node()"/>
    <xsl:text>"</xsl:text>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>}</xsl:text>
    <xsl:if test="position() = last()">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>]</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="mc:policy">
    <xsl:param name="level"/>
    <xsl:param name="attributes"/>
    <xsl:if test="$attributes or preceding-sibling::*">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:if test="position() = 1">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>"policies": [</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>{</xsl:text>
    <xsl:if test="@name">
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"name": "</xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@type">
      <xsl:if test="@name">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"type": "</xsl:text>
      <xsl:value-of select="@type"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@level">
      <xsl:if test="@name or @type">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"level": "</xsl:text>
      <xsl:value-of select="@level"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@rules_run">
      <xsl:if test="@name or @type or @level">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"rules_run": </xsl:text>
      <xsl:value-of select="@rules_run"/>
    </xsl:if>
    <xsl:if test="@pass_count">
      <xsl:if test="@name or @type or @level or @rules_run">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"pass_count": </xsl:text>
      <xsl:value-of select="@pass_count"/>
    </xsl:if>
    <xsl:if test="@info_count">
      <xsl:if test="@name or @type or @level or @rules_run or @pass_count">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"info_count": </xsl:text>
      <xsl:value-of select="@info_count"/>
    </xsl:if>
    <xsl:if test="@warn_count">
      <xsl:if test="@name or @type or @level or @rules_run or @pass_count or @info_count">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"warn_count": </xsl:text>
      <xsl:value-of select="@warn_count"/>
    </xsl:if>
    <xsl:if test="@fail_count">
      <xsl:if test="@name or @type or @level or @rules_run or @pass_count or @info_count or @warn_count">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"fail_count": </xsl:text>
      <xsl:value-of select="@fail_count"/>
    </xsl:if>
    <xsl:if test="@outcome">
      <xsl:if test="@name or @type or @level or @rules_run or @pass_count or @info_count or @warn_count or @fail_count">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"outcome": "</xsl:text>
      <xsl:value-of select="@outcome"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:apply-templates select="mc:description">
      <xsl:with-param name="level" select="$level + 2"/>
      <xsl:with-param name="attributes" select="@name or @type or @level or @rules_run or @pass_count or @info_count or @warn_count or @fail_count or @outcome"/>
    </xsl:apply-templates>
    <xsl:apply-templates select="mc:tag">
      <xsl:with-param name="level" select="$level + 2"/>
      <xsl:with-param name="attributes" select="@name or @type or @level or @rules_run or @pass_count or @info_count or @warn_count or @fail_count or @outcome"/>
    </xsl:apply-templates>
    <xsl:apply-templates select="mc:policy">
      <xsl:with-param name="level" select="$level + 2"/>
      <xsl:with-param name="attributes" select="@name or @type or @level or @rules_run or @pass_count or @info_count or @warn_count or @fail_count or @outcome"/>
    </xsl:apply-templates>
    <xsl:apply-templates select="mc:rule">
      <xsl:with-param name="level" select="$level + 2"/>
      <xsl:with-param name="attributes" select="@name or @type or @level or @rules_run or @pass_count or @info_count or @warn_count or @fail_count or @outcome"/>
    </xsl:apply-templates>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>}</xsl:text>
    <xsl:if test="position() = last()">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>]</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="mc:rule">
    <xsl:param name="level"/>
    <xsl:param name="attributes"/>
    <xsl:if test="$attributes or preceding-sibling::*">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:if test="position() = 1">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>"rules": [</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>{</xsl:text>
    <xsl:if test="@name">
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"name": "</xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@scope">
      <xsl:if test="@name">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"scope": "</xsl:text>
      <xsl:value-of select="@scope"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@level">
      <xsl:if test="@name or @scope">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"level": "</xsl:text>
      <xsl:value-of select="@level"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@value">
      <xsl:if test="@name or @scope or @level">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"value": "</xsl:text>
      <xsl:value-of select="@value"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@tracktype">
      <xsl:if test="@name or @scope or @level or @value">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"tracktype": "</xsl:text>
      <xsl:value-of select="@tracktype"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@occurrence">
      <xsl:if test="@name or @scope or @level or @value or @tracktype">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"occurrence": "</xsl:text>
      <xsl:value-of select="@occurrence"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@operator">
      <xsl:if test="@name or @scope or @level or @value or @tracktype or @occurrence">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"operator": "</xsl:text>
      <xsl:value-of select="@operator"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@xpath">
      <xsl:if test="@name or @scope or @level or @value or @tracktype or @occurrence or @operator">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"xpath": "</xsl:text>
      <xsl:value-of select="@xpath"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@requested">
      <xsl:if test="@name or @scope or @level or @value or @tracktype or @occurrence or @operator or @xpath">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"requested": "</xsl:text>
      <xsl:value-of select="@requested"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@actual">
      <xsl:if test="@name or @scope or @level or @value or @tracktype or @occurrence or @operator or @xpath or @requested">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"actual": "</xsl:text>
      <xsl:value-of select="@actual"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@outcome">
      <xsl:if test="@name or @scope or @level or @value or @tracktype or @occurrence or @operator or @xpath or @requested or @actual">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 4 + $level * 2)"/>
      <xsl:text>"outcome": "</xsl:text>
      <xsl:value-of select="@outcome"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:apply-templates select="mc:source">
      <xsl:with-param name="level" select="$level + 2"/>
      <xsl:with-param name="attributes" select="@name or @scope or @level or @value or @tracktype or @occurrence or @operator or @xpath or @requested or @actual or @outcome"/>
    </xsl:apply-templates>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>}</xsl:text>
    <xsl:if test="position() = last()">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>]</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="mc:source">
    <xsl:param name="level"/>
    <xsl:param name="attributes"/>
    <xsl:if test="$attributes or preceding-sibling::*">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
    <xsl:text>"source": {</xsl:text>
    <xsl:if test="@occurrence">
      <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
      <xsl:text>"occurrence": "</xsl:text>
      <xsl:value-of select="@occurrence"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@scope">
      <xsl:if test="@occurrence">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
      <xsl:text>"scope": "</xsl:text>
      <xsl:value-of select="@scope"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@tracktype">
      <xsl:if test="@occurrence or @scope">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
      <xsl:text>"tracktype": "</xsl:text>
      <xsl:value-of select="@tracktype"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@value">
      <xsl:if test="@occurrence or @scope or @tracktype">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
      <xsl:text>"value": "</xsl:text>
      <xsl:value-of select="@value"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
    <xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="mc:tag">
    <xsl:param name="level"/>
    <xsl:param name="attributes"/>
    <xsl:if test="$attributes or preceding-sibling::*">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:if test="position() = 1">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>"tags": [</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>"</xsl:text>
    <xsl:value-of select="node()"/>
    <xsl:text>"</xsl:text>
    <xsl:if test="position() = last()">
      <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
      <xsl:text>]</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="mc:creatingApplication|mc:creatingLibrary">
    <xsl:param name="level"/>
    <xsl:param name="attributes"/>
    <xsl:if test="$attributes or preceding-sibling::*">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
    <xsl:text>"</xsl:text>
    <xsl:value-of select="name()"/>
    <xsl:text>": {</xsl:text>
    <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
    <xsl:text>"name": "</xsl:text>
    <xsl:value-of select="node()"/>
    <xsl:text>"</xsl:text>
    <xsl:if test="@version">
      <xsl:text>,</xsl:text>
      <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
      <xsl:text>"version": "</xsl:text>
      <xsl:value-of select="@version"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@url">
      <xsl:text>,</xsl:text>
      <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
      <xsl:text>"url": "</xsl:text>
      <xsl:value-of select="@url"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@build_date">
      <xsl:text>,</xsl:text>
      <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
      <xsl:text>"build_date": "</xsl:text>
      <xsl:value-of select="@build_date"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@build_time">
      <xsl:text>,</xsl:text>
      <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
      <xsl:text>"build_time": "</xsl:text>
      <xsl:value-of select="@build_time"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:if test="@compiler_ident">
      <xsl:text>,</xsl:text>
      <xsl:value-of select="substring($indent, 1, 1 + 2 + $level * 2)"/>
      <xsl:text>"compiler_ident": "</xsl:text>
      <xsl:value-of select="@compiler_ident"/>
      <xsl:text>"</xsl:text>
    </xsl:if>
    <xsl:value-of select="substring($indent, 1, 1 + $level * 2)"/>
    <xsl:text>}</xsl:text>
  </xsl:template>
</xsl:stylesheet>
