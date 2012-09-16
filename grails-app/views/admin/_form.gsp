<%@ page import="eu.fiveminutes.lmi.Door" %>

<div class="fieldcontain ${hasErrors(bean: doorInstance, field: 'pin', 'error')} required">
	<label for="pin">
		<g:message code="door.pin.label" default="Pin" />
		<span class="required-indicator">*</span>
	</label>
	<g:textField name="pin" required="" value="${doorInstance?.pin}"/>
</div>

<div class="fieldcontain">
    <label for="password">
        <g:message code="new.password.label" default="New password" />
    </label>
    <g:passwordField name="password" />
</div>

<div class="fieldcontain">
    <label for="password2">
        <g:message code="confirm.password.label" default="Confirm new password" />
    </label>
    <g:passwordField name="password2" />
</div>

