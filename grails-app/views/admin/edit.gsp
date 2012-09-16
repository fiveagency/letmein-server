<%@ page import="eu.fiveminutes.lmi.Door" %>
<!doctype html>
<html>
    <head>
        <meta name="layout" content="main">
    </head>
    <body>
        <a href="#edit-door" class="skip" tabindex="-1"><g:message code="default.link.skip.label" default="Skip to content&hellip;"/></a>
        <div id="edit-door" class="content scaffold-edit" role="main">
            <h1><g:message code="edit.settings.label" default="Edit settings" /></h1>
            <g:if test="${flash.message}">
            <div class="message" role="status">${flash.message}</div>
            </g:if>
            <g:if test="${flash.error}">
            <ul class="errors"><li>${flash.error}</li></ul>
            </g:if>
            <g:hasErrors bean="${doorInstance}">
            <ul class="errors" role="alert">
                <g:eachError bean="${doorInstance}" var="error">
                <li <g:if test="${error in org.springframework.validation.FieldError}">data-field-id="${error.field}"</g:if>><g:message error="${error}"/></li>
                </g:eachError>
            </ul>
            </g:hasErrors>
            <g:form method="post" >
                <g:hiddenField name="id" value="${doorInstance?.id}" />
                <g:hiddenField name="version" value="${doorInstance?.version}" />
                <fieldset class="form">
                    <g:render template="form"/>
                </fieldset>
                <fieldset class="buttons">
                    <g:actionSubmit class="save" action="update" value="${message(code: 'default.button.update.label', default: 'Update')}" />
                </fieldset>
            </g:form>
        </div>
    </body>
</html>
