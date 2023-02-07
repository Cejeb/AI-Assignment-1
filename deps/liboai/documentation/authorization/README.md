<h1>Authorization</h1>
<p>The <code>Authorization</code> class is defined in <code>authorization.h</code> at <code>liboai::Authorization</code>. This class is responsible for sharing all set authorization information with all component classes in <code>liboai</code>.

All authorization information should be set prior to the calling of any component methods such as <code>Images</code>, <code>Embeddings</code>, and so on. Failure to do so will result in a <code>liboai::OpenAIException</code> due to authorization failure on OpenAI's end.</p>

<br>
<h2>Methods</h2>
<p>This document covers the method(s) located in <code>authorization.h</code>. You can find their function signature(s) below.</p>

<h3>Get Authorizer</h3>
<p>Returns a reference to the <code>liboai::Authorization</code> singleton shared among all components.</p>

```cpp
static Authorization& Authorizer();
```

<h3>Set API Key</h3>
<p>Sets the API key to use in subsequent component calls.</p>

```cpp
bool SetKey(const std::string& key);
bool SetKey(std::string&& key);
```

<h3>Set API Key (File)</h3>
<p>Sets the API key to use in subsequent component calls from data found in file at path.</p>

```cpp
bool SetKeyFile(const std::filesystem::path& path);
```

<h3>Set API Key (Environment Variable)</h3>
<p>Sets the API key to use in subsequent component calls from an environment variable.</p>

```cpp
bool SetKeyEnv(const std::string& var);
```

<h3>Set Organization ID</h3>
<p>Sets the organization ID to send in subsequent component calls.</p>

```cpp
bool SetOrganization(const std::string& org);
bool SetOrganization(std::string&& org);
```

<h3>Set Organization ID (File)</h3>
<p>Sets the organization ID to send in subsequent component calls from data found in file at path.</p>

```cpp
bool SetOrganizationFile(const std::filesystem::path& path);
```

<h3>Set Organization ID (Environment Variable)</h3>
<p>Sets the organization ID to send in subsequent component calls from an environment variable.</p>

```cpp
bool SetOrganizationEnv(const std::string& var);
```

<h3>Set Proxies</h3>
<p>Sets the proxy, or proxies, to use in subsequent component calls.</p>

```cpp
void SetProxies(const std::initializer_list<std::pair<const std::string, std::string>>& hosts);
void SetProxies(std::initializer_list<std::pair<const std::string, std::string>>&& hosts);
void SetProxies(const std::map<std::string, std::string>& hosts);
void SetProxies(std::map<std::string, std::string>&& hosts);
```

<h3>Get Key</h3>
<p>Returns the currently set API key.</p>

```cpp
constexpr const std::string& GetKey() const;
```

<h3>Get Organization ID</h3>
<p>Returns the currently set organization ID.</p>

```cpp
constexpr const std::string& GetOrganization() const;
```


<h3>Get Proxies</h3>
<p>Returns the currently set proxies.</p>

```cpp
cpr::Proxies GetProxies();
```

<h3>Get Authorization Headers</h3>
<p>Returns the currently set authorization headers based on set information.</p>

```cpp
constexpr const cpr::Header& GetAuthorizationHeaders() const;
```

<br>
<h2>Example Usage</h2>
<p>For example usage of the above function(s), please refer to the <a href="./examples">examples</a> folder.
