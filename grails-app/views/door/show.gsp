
<%@ page import="eu.fiveminutes.lmi.Door" %>
<!doctype html>
<html>
	<head>
		<meta name="layout" content="main">
		<g:set var="entityName" value="${message(code: 'door.label', default: 'Door')}" />
		<title><g:message code="default.show.label" args="[entityName]" /></title>
	</head>
	<body>
		<a href="#show-door" class="skip" tabindex="-1"><g:message code="default.link.skip.label" default="Skip to content&hellip;"/></a>
		<div id="show-door" class="content scaffold-show" role="main">
			<h1><g:message code="default.show.label" args="[entityName]" /></h1>
			<g:if test="${flash.message}">
			<div class="message" role="status">${flash.message}</div>
			</g:if>
			<ol class="property-list door">
			
				<g:if test="${doorInstance?.pin}">
				<li class="fieldcontain">
					<span id="pin-label" class="property-label"><g:message code="door.pin.label" default="Pin" /></span>
					
						<span class="property-value" aria-labelledby="pin-label"><g:fieldValue bean="${doorInstance}" field="pin"/></span>
					
				</li>
				</g:if>
			
			</ol>
			<g:form>
				<fieldset class="buttons">
					<g:hiddenField name="id" value="${doorInstance?.id}" />
					<g:link class="edit" action="edit" id="${doorInstance?.id}"><g:message code="default.button.edit.label" default="Edit" /></g:link>
				</fieldset>
			</g:form>
		</div>
	</body>
</html>
