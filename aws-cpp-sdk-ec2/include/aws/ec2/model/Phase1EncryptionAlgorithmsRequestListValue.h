﻿/*
* Copyright 2010-2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License").
* You may not use this file except in compliance with the License.
* A copy of the License is located at
*
*  http://aws.amazon.com/apache2.0
*
* or in the "license" file accompanying this file. This file is distributed
* on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied. See the License for the specific language governing
* permissions and limitations under the License.
*/

#pragma once
#include <aws/ec2/EC2_EXPORTS.h>
#include <aws/core/utils/memory/stl/AWSStreamFwd.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <utility>

namespace Aws
{
namespace Utils
{
namespace Xml
{
  class XmlNode;
} // namespace Xml
} // namespace Utils
namespace EC2
{
namespace Model
{

  /**
   * <p>Specifies the encryption algorithm for the VPN tunnel for phase 1 IKE
   * negotiations.</p><p><h3>See Also:</h3>   <a
   * href="http://docs.aws.amazon.com/goto/WebAPI/ec2-2016-11-15/Phase1EncryptionAlgorithmsRequestListValue">AWS
   * API Reference</a></p>
   */
  class AWS_EC2_API Phase1EncryptionAlgorithmsRequestListValue
  {
  public:
    Phase1EncryptionAlgorithmsRequestListValue();
    Phase1EncryptionAlgorithmsRequestListValue(const Aws::Utils::Xml::XmlNode& xmlNode);
    Phase1EncryptionAlgorithmsRequestListValue& operator=(const Aws::Utils::Xml::XmlNode& xmlNode);

    void OutputToStream(Aws::OStream& ostream, const char* location, unsigned index, const char* locationValue) const;
    void OutputToStream(Aws::OStream& oStream, const char* location) const;


    /**
     * <p>The value for the encryption algorithm.</p>
     */
    inline const Aws::String& GetValue() const{ return m_value; }

    /**
     * <p>The value for the encryption algorithm.</p>
     */
    inline bool ValueHasBeenSet() const { return m_valueHasBeenSet; }

    /**
     * <p>The value for the encryption algorithm.</p>
     */
    inline void SetValue(const Aws::String& value) { m_valueHasBeenSet = true; m_value = value; }

    /**
     * <p>The value for the encryption algorithm.</p>
     */
    inline void SetValue(Aws::String&& value) { m_valueHasBeenSet = true; m_value = std::move(value); }

    /**
     * <p>The value for the encryption algorithm.</p>
     */
    inline void SetValue(const char* value) { m_valueHasBeenSet = true; m_value.assign(value); }

    /**
     * <p>The value for the encryption algorithm.</p>
     */
    inline Phase1EncryptionAlgorithmsRequestListValue& WithValue(const Aws::String& value) { SetValue(value); return *this;}

    /**
     * <p>The value for the encryption algorithm.</p>
     */
    inline Phase1EncryptionAlgorithmsRequestListValue& WithValue(Aws::String&& value) { SetValue(std::move(value)); return *this;}

    /**
     * <p>The value for the encryption algorithm.</p>
     */
    inline Phase1EncryptionAlgorithmsRequestListValue& WithValue(const char* value) { SetValue(value); return *this;}

  private:

    Aws::String m_value;
    bool m_valueHasBeenSet;
  };

} // namespace Model
} // namespace EC2
} // namespace Aws
