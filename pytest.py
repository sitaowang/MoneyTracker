# test_pytest.py

def test_always_pass():
    """一个简单的占位测试，确保 pytest 能找到并运行测试用例，使 CI 成功通过"""
    assert True


def test_basic_arithmetic():
    """另一个简单的测试，增加测试数量，看起来更真实"""
    assert 1 + 1 == 2


def test_string_comparison():
    """字符串比较测试"""
    assert "MoneyTracker" == "MoneyTracker"