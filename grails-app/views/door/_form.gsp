<%@ page import="eu.fiveminutes.lmi.Door" %>



<div class="fieldcontain ${hasErrors(bean: doorInstance, field: 'pin', 'error')} required">
	<label for="pin">
		<g:message code="door.pin.label" default="Pin" />
		<span class="required-indicator">*</span>
	</label>
	<g:textField name="pin" required="" value="${doorInstance?.pin}"/>
</div>

